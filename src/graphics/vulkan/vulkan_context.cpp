#include "vulkan_context.h"
#include "core/logger.h"
#include "platform/window.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>
#include <cstring>

namespace ember::graphics::vulkan {

VulkanContext::VulkanContext()
    : instance_(VK_NULL_HANDLE),
      physicalDevice_(VK_NULL_HANDLE),
      device_(VK_NULL_HANDLE),
      graphicsQueue_(VK_NULL_HANDLE),
      graphicsQueueFamily_(0),
      surface_(VK_NULL_HANDLE),
      initialized_(false) {
}

VulkanContext::~VulkanContext() {
    if (initialized_) {
        shutdown();
    }
}

bool VulkanContext::initialize(platform::Window* window) {
    if (initialized_) {
        EMBER_LOG_WARN("VulkanContext already initialized");
        return true;
    }

    try {
        // Create Vulkan instance
        if (!createInstance()) {
            EMBER_LOG_ERROR("Failed to create Vulkan instance");
            return false;
        }

        // Create surface if window provided (for desktop rendering)
        if (window) {
            if (!createSurface(window)) {
                EMBER_LOG_ERROR("Failed to create Vulkan surface");
                return false;
            }
        }

        // Select physical device
        if (!selectPhysicalDevice()) {
            EMBER_LOG_ERROR("Failed to select physical device");
            return false;
        }

        // Create logical device
        if (!createLogicalDevice()) {
            EMBER_LOG_ERROR("Failed to create logical device");
            return false;
        }

        // Get graphics queue
        vkGetDeviceQueue(device_, graphicsQueueFamily_, 0, &graphicsQueue_);

        initialized_ = true;
        EMBER_LOG_INFO("VulkanContext initialized successfully");
        return true;

    } catch (const std::exception& e) {
        EMBER_LOG_ERROR("VulkanContext initialization exception: " + std::string(e.what()));
        shutdown();
        return false;
    }
}

void VulkanContext::shutdown() {
    if (!initialized_) {
        return;
    }

    if (device_ != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(device_);
        vkDestroyDevice(device_, nullptr);
        device_ = VK_NULL_HANDLE;
    }

    if (surface_ != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(instance_, surface_, nullptr);
        surface_ = VK_NULL_HANDLE;
    }

    if (instance_ != VK_NULL_HANDLE) {
        vkDestroyInstance(instance_, nullptr);
        instance_ = VK_NULL_HANDLE;
    }

    physicalDevice_ = VK_NULL_HANDLE;
    graphicsQueue_ = VK_NULL_HANDLE;
    graphicsQueueFamily_ = 0;
    initialized_ = false;

    EMBER_LOG_INFO("VulkanContext shutdown complete");
}

std::shared_ptr<VulkanContext> VulkanContext::getShared() {
    return shared_from_this();
}

bool VulkanContext::createInstance() {
    // Application info
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Ember Engine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Ember";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    // Instance create info
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // Get required extensions
    std::vector<const char*> extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    // Validation layers
    std::vector<const char*> validationLayers;
#ifdef EMBER_DEBUG
    validationLayers.push_back("VK_LAYER_KHRONOS_validation");
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
#else
    createInfo.enabledLayerCount = 0;
#endif

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance_);
    if (result != VK_SUCCESS) {
        EMBER_LOG_ERROR("vkCreateInstance failed with code: " + std::to_string(result));
        return false;
    }

    return true;
}

bool VulkanContext::createSurface(platform::Window* window) {
    if (!window) {
        EMBER_LOG_WARN("Window is null, skipping surface creation");
        return true;  // Headless mode is acceptable
    }

    // Platform-specific surface creation
    // This is a stub - actual implementation depends on platform layer
    // For now, we'll create a simple surface placeholder

    EMBER_LOG_INFO("Surface creation deferred to platform layer");
    return true;
}

bool VulkanContext::selectPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);

    if (deviceCount == 0) {
        EMBER_LOG_ERROR("No physical devices found");
        return false;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance_, &deviceCount, devices.data());

    // Select first suitable device
    for (const auto& device : devices) {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(device, &props);

        EMBER_LOG_INFO("Found device: " + std::string(props.deviceName));

        // Check for graphics queue support
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        for (uint32_t i = 0; i < queueFamilyCount; i++) {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                physicalDevice_ = device;
                graphicsQueueFamily_ = i;
                EMBER_LOG_INFO("Selected device: " + std::string(props.deviceName));
                return true;
            }
        }
    }

    EMBER_LOG_ERROR("No suitable physical device found");
    return false;
}

bool VulkanContext::createLogicalDevice() {
    // Queue create info
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = graphicsQueueFamily_;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    // Device features
    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.multiViewport = VK_TRUE;

    // Device create info
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.pEnabledFeatures = &deviceFeatures;

    // Extensions
    std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    VkResult result = vkCreateDevice(physicalDevice_, &createInfo, nullptr, &device_);
    if (result != VK_SUCCESS) {
        EMBER_LOG_ERROR("vkCreateDevice failed with code: " + std::to_string(result));
        return false;
    }

    return true;
}

std::vector<const char*> VulkanContext::getRequiredExtensions() {
    std::vector<const char*> extensions;

    // Core extensions
    extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

    // Platform-specific surface extension
#ifdef VK_USE_PLATFORM_WIN32_KHR
    extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
    extensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XCBKHR)
    extensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_METAL_EXT)
    extensions.push_back(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
#endif

    return extensions;
}

// Getters
VkInstance VulkanContext::getInstance() const {
    return instance_;
}

VkPhysicalDevice VulkanContext::getPhysicalDevice() const {
    return physicalDevice_;
}

VkDevice VulkanContext::getDevice() const {
    return device_;
}

VkQueue VulkanContext::getGraphicsQueue() const {
    return graphicsQueue_;
}

uint32_t VulkanContext::getGraphicsQueueFamily() const {
    return graphicsQueueFamily_;
}

VkSurfaceKHR VulkanContext::getSurface() const {
    return surface_;
}

}  // namespace ember::graphics::vulkan
