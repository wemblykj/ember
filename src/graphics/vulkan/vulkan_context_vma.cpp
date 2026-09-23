#include "vulkan_context_vma.h"

#include <cstring>
#include <stdexcept>
#include <vector>

#include <vulkan/vulkan.h>

namespace ember::graphics::vulkan {

VulkanContextVma::VulkanContextVma(const RendererConfig& config)
    : config_(config),
      instance_(VK_NULL_HANDLE),
      physicalDevice_(VK_NULL_HANDLE),
      device_(VK_NULL_HANDLE),
      graphicsQueue_(VK_NULL_HANDLE),
      graphicsQueueFamily_(0),
      surface_(VK_NULL_HANDLE),
      initialized_(false) {
}

VulkanContextVma::~VulkanContextVma() {
    if (initialized_) {
        shutdown();
    }
}

bool VulkanContextVma::initialize(VulkanSurfaceProvider* surfaceProvider) {
    if (initialized_) {
        EMBER_LOG_WARN("VulkanContextVma already initialized");
        return true;
    }

    try {
        // Get required extensions
        std::vector<const char*> extensions = getRequiredExtensions();
        if (surfaceProvider) {
            std::vector<const char*> surfaceExtensions = surfaceProvider->getRequiredInstanceExtensions();
			extensions.insert(extensions.end(), surfaceExtensions.begin(), surfaceExtensions.end());
        }
        // Create Vulkan instance
        if (!createInstance(extensions)) {
            EMBER_LOG_ERROR("Failed to create Vulkan instance");
            return false;
        }

        // Create surface if window provided (for desktop rendering)
        if (surfaceProvider) {
            if (!createSurface(surfaceProvider)) {
                EMBER_LOG_ERROR("Failed to create Vulkan surface");
                return false;
            }
        }
        else {
            EMBER_LOG_WARN("SurfaceProvider is null, skipping surface creation");
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

        if (!createMemoryAllocator()) {
            EMBER_LOG_ERROR("Failed to create Vulkan memory allocator");
            return false;
		}

        // Get graphics queue
        vkGetDeviceQueue(device_, graphicsQueueFamily_, 0, &graphicsQueue_);

        initialized_ = true;
        EMBER_LOG_INFO("VulkanContextVma initialized successfully");
        return true;

    } catch (const std::exception& e) {
        EMBER_LOG_ERROR("VulkanContextVma initialization exception: {}", e.what());
        shutdown();
        return false;
    }
}

void VulkanContextVma::shutdown() {
    if (!initialized_) {
        return;
    }

    if (allocator_ != VK_NULL_HANDLE) {
        vmaDestroyAllocator(allocator_);
        allocator_ = VK_NULL_HANDLE;
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

    EMBER_LOG_INFO("VulkanContextVma shutdown complete");
}

VkResult VulkanContextVma::createCommandPool(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags,
	VkCommandPool* outPool)
{
    VkCommandPoolCreateInfo info{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    	.flags = flags,
		.queueFamilyIndex = queueFamilyIndex
    };

	return vkCreateCommandPool(device_, &info, nullptr, outPool);
}

void VulkanContextVma::destroyCommandPool(VkCommandPool pool)
{
	vkDestroyCommandPool(device_, pool, nullptr);
}

VkResult VulkanContextVma::allocateCommandBuffers(VkCommandPool pool, VkCommandBufferLevel level, uint32_t count,
	VkCommandBuffer* outBuffers)
{
    VkCommandBufferAllocateInfo info{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = pool,
        .level = level,
        .commandBufferCount = count
    };

	return vkAllocateCommandBuffers(device_, &info, outBuffers);
}

void VulkanContextVma::freeCommandBuffers(VkCommandPool pool, uint32_t count, const VkCommandBuffer* buffers)
{
	vkFreeCommandBuffers(device_, pool, count, buffers);
}

VkCommandBuffer VulkanContextVma::beginOneTimeCommands()
{
	
}

void VulkanContextVma::endOneTimeCommands(VkCommandBuffer cmd)
{
	
}

VkResult VulkanContextVma::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer* outBuffer, VmaAllocation* outMemory)
{
    VkBuffer buffer;
    VkDeviceMemory memory;

    VkBufferCreateInfo info{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.flags = 0,
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };
	
    return vmaCreateBuffer(allocator_, &info, nullptr, outBuffer, outMemory, nullptr);
	
}

void VulkanContextVma::destroyBuffer(VkBuffer buffer, VkDeviceMemory memory)
{
	vkDestroyBuffer(device_, buffer, nullptr);
	vkFreeMemory(device_, memory, nullptr);
}

bool VulkanContextVma::createInstance(std::vector<const char*> extensions) {
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
        EMBER_LOG_ERROR("vkCreateInstance failed with code: {}", std::to_string(result));
        return false;
    }

    return true;
}

bool VulkanContextVma::createSurface(VulkanSurfaceProvider* surfaceProvider) {
    // Platform-specific surface creation
    // This is a stub - actual implementation depends on platform layer
    // For now, we'll create a simple surface placeholder
	
    EMBER_LOG_INFO("Surface creation deferred to platform layer");

    return surfaceProvider->createSurface(instance_, surface_);
}

bool VulkanContextVma::createMemoryAllocator()
{
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.physicalDevice = physicalDevice_;
    allocatorInfo.device = device_;
    allocatorInfo.instance = instance_;
    allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_2;

    VkResult result = vmaCreateAllocator(&allocatorInfo, &allocator_);
    if (result != VK_SUCCESS) {
        EMBER_LOG_ERROR("vmaCreateAllocator failed with code: {}", std::to_string(result));
        return false;
    }
}

bool VulkanContextVma::selectPhysicalDevice() {
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

bool VulkanContextVma::createLogicalDevice() {
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

std::vector<const char*> VulkanContextVma::getRequiredExtensions() {
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

}  // namespace ember::graphics::vulkan
