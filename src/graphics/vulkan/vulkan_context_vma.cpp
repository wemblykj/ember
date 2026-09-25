#include "vulkan_context_vma.h"

#define VMA_IMPLEMENTATION
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include <vk_mem_alloc.h>

#include <algorithm>

#include "any_physical_device_selector.h"

namespace ember::graphics::vulkan {

VulkanContextVma::VulkanContextVma() {
}

VulkanContextVma::~VulkanContextVma() {
    if (initialized_) {
        destroy();
    }
}

bool VulkanContextVma::createDefaultInstance(VkInstance& instance, const ExtensionSet& requiredExtensions) {
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

    createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());

    std::vector<const char*> extensionNames;
	std::ranges::transform(requiredExtensions, std::back_inserter(extensionNames), [](const std::string_view& ext) { return ext.data(); });
    createInfo.ppEnabledExtensionNames = extensionNames.data();

    // Validation layers
    std::vector<const char*> validationLayers;
#ifdef EMBER_DEBUG
    validationLayers.push_back("VK_LAYER_KHRONOS_validation");
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
#else
    createInfo.enabledLayerCount = 0;
#endif

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS) {
        EMBER_LOG_ERROR("vkCreateInstance failed with code: {}", std::to_string(result));
        return false;
    }

    return true;
}

bool VulkanContextVma::initialize(const ExtensionSet& extensions, PhysicalDeviceSelectorPtr deviceSelector) {
    if (initialized_) {
        EMBER_LOG_WARN("VulkanContextVma already initialized");
        return false;
	}

    VkInstance instance;
	if (!createDefaultInstance(instance, extensions)) {
        EMBER_LOG_ERROR("Failed to create Vulkan instance");
        return false;
	}

    return initialize(instance, std::move(deviceSelector));
}

bool VulkanContextVma::initialize(VkInstance instance, PhysicalDeviceSelectorPtr deviceSelector) {
    if (initialized_) {
        EMBER_LOG_WARN("VulkanContextVma already initialized");
        return false;
    }

	instance_ = instance;

    if (!deviceSelector) deviceSelector = createAnyPhysicalDeviceSelector();
	
    try {   
        // Select physical device
		auto deviceSelection = deviceSelector->select(instance_);
        if (!deviceSelection) {
            EMBER_LOG_ERROR("Failed to select physical device");
            return false;
        }

        EMBER_LOG_INFO("Selected physical device: {}", deviceSelection->properties.deviceName);

		physicalDevice_ = deviceSelection->physicalDevice;
		graphicsQueueFamily_ = deviceSelection->queueFamily;

        // Create logical device
        if (!createLogicalDevice()) {
            EMBER_LOG_ERROR("Failed to create logical device");
            return false;
        }

        if (!createMemoryAllocator()) {
            EMBER_LOG_ERROR("Failed to create Vulkan memory allocator");
            return false;
        }

        // Create one time command pool
        VkResult result = createCommandPool(graphicsQueueFamily_, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, &commandPool_);
        if (result != VK_SUCCESS) {
            EMBER_LOG_ERROR("Failed to create one time command pool: {}", std::to_string(result));
            return false;
        }

        // Get graphics queue
        vkGetDeviceQueue(device_, graphicsQueueFamily_, 0, &graphicsQueue_);

        initialized_ = true;

        EMBER_LOG_INFO("VulkanContextVma initialized successfully");
        return true;

    }
    catch (const std::exception& e) {
        EMBER_LOG_ERROR("VulkanContextVma initialization exception: {}", e.what());
        destroy();
        return false;
    }
}

void VulkanContextVma::destroy() {
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

    if (instance_ != VK_NULL_HANDLE) {
        vkDestroyInstance(instance_, nullptr);
        instance_ = VK_NULL_HANDLE;
    }

	destroyCommandPool(commandPool_);

    physicalDevice_ = VK_NULL_HANDLE;
    graphicsQueue_ = VK_NULL_HANDLE;
    graphicsQueueFamily_ = 0;
    initialized_ = false;

    EMBER_LOG_INFO("VulkanContextVma shutdown complete");
}

void VulkanContextVma::waitIdle() {
    vkDeviceWaitIdle(device_);
}

VkResult VulkanContextVma::createCommandPool(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags,
                                             VkCommandPool* outPool) {
    VkCommandPoolCreateInfo info{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    	.flags = flags,
		.queueFamilyIndex = queueFamilyIndex
    };

	return vkCreateCommandPool(device_, &info, nullptr, outPool);
}

void VulkanContextVma::destroyCommandPool(VkCommandPool pool) {
	vkDestroyCommandPool(device_, pool, nullptr);
}

VkResult VulkanContextVma::allocateCommandBuffers(VkCommandPool pool, VkCommandBufferLevel level, uint32_t count,
	VkCommandBuffer* outBuffers) {
    VkCommandBufferAllocateInfo info{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = pool,
        .level = level,
        .commandBufferCount = count
    };

	return vkAllocateCommandBuffers(device_, &info, outBuffers);
}

void VulkanContextVma::freeCommandBuffers(VkCommandPool pool, uint32_t count, const VkCommandBuffer* buffers) {
	vkFreeCommandBuffers(device_, pool, count, buffers);
}

VkCommandBuffer VulkanContextVma::beginOneTimeCommands() {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool_;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device_, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);
    
	return commandBuffer;
}

void VulkanContextVma::endOneTimeCommands(VkCommandBuffer cmd) {
	vkEndCommandBuffer(cmd);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmd;

	vkQueueSubmit(graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue_);

	vkFreeCommandBuffers(device_, commandPool_, 1, &cmd);
}

VkResult VulkanContextVma::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer* outBuffer, AllocationHandle* outMemory) {
    VkBufferCreateInfo bufferInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.flags = 0,
        .size = size,
        .usage = usage
    };
	
    VmaAllocationCreateInfo allocInfo = {
        .usage = VMA_MEMORY_USAGE_AUTO
    };
    
    VmaAllocation allocation;
    VkResult result = vmaCreateBuffer(allocator_, &bufferInfo, &allocInfo, outBuffer, &allocation, nullptr);
    if (result == VK_SUCCESS) {
        *outMemory = toAllocationHandle(allocation);
    }

    return result;
}

void VulkanContextVma::destroyBuffer(VkBuffer buffer, AllocationHandle memory) {
    vmaDestroyBuffer(allocator_, buffer, toVma(memory));
}

VkResult VulkanContextVma::createImage(VkDeviceSize size, VkImageType type, VkImageUsageFlags usage, VkImage* outImage, AllocationHandle* outMemory)
{
    VkImageCreateInfo bufferInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .flags = 0,
        .imageType = type,
        .usage = usage
    };

    VmaAllocationCreateInfo allocInfo = {
        .usage = VMA_MEMORY_USAGE_AUTO
    };

    VmaAllocation allocation;
    VkResult result = vmaCreateImage(allocator_, &bufferInfo, &allocInfo, outImage, &allocation, nullptr);
    if (result == VK_SUCCESS) {
        *outMemory = toAllocationHandle(allocation);
    }

    return result;
}

bool VulkanContextVma::createMemoryAllocator() {
    // fetching pointers to Vulkan functions dynamically
    // https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/quick_start.html
    VmaVulkanFunctions vulkanFunctions = {
        .vkGetInstanceProcAddr = &vkGetInstanceProcAddr,
		.vkGetDeviceProcAddr = &vkGetDeviceProcAddr
    };
    
    VmaAllocatorCreateInfo allocatorCreateInfo = {};
    allocatorCreateInfo.physicalDevice = physicalDevice_;
    allocatorCreateInfo.device = device_;
    allocatorCreateInfo.instance = instance_;
    allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_2;
    allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;

    VkResult result = vmaCreateAllocator(&allocatorCreateInfo, &allocator_);
    if (result != VK_SUCCESS) {
        EMBER_LOG_ERROR("vmaCreateAllocator failed with code: {}", std::to_string(result));
        return false;
    }

    return true;
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

}  // namespace ember::graphics::vulkan
