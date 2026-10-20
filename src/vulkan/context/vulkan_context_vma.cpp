#include "vulkan_context_vma.h"

#define VMA_IMPLEMENTATION
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include <vk_mem_alloc.h>

#include <algorithm>
#include <ranges>
#include <vector>

#include <logger.h>

#include "any_physical_device_selector.h"

namespace ember::vulkan {

VulkanContextVma::VulkanContextVma(const VulkanInstancePtr& instance)
    : instance_(instance) {
}

VulkanContextVma::~VulkanContextVma() {
    if (initialized_) {
        destroy();
    }
}

bool VulkanContextVma::initialize(const PhysicalDevicePtr& physicalDevice) {
    if (initialized_) {
        EMBER_LOG_WARN("VulkanContextVma already initialized");
        return false;
    }

	physicalDevice_ = physicalDevice;

    try {   
        // Create logical device
        if (!createLogicalDevice()) {
            EMBER_LOG_ERROR("Failed to create logical device");
            return false;
        }

        if (!createMemoryAllocator()) {
            EMBER_LOG_ERROR("Failed to create Vulkan memory allocator");
            return false;
        }

        auto graphicQueueuIt = std::find_if(physicalDevice_->traits.queueFamilies.begin(), physicalDevice_->traits.queueFamilies.end(),
            [](const QueueFamilyTraits& qf) { return (qf.flags & VK_QUEUE_GRAPHICS_BIT) != 0; });

        if (graphicQueueuIt != physicalDevice_->traits.queueFamilies.end()) {
             const auto graphicsQueueFamilyIndex = graphicQueueuIt->queueFamilyIndex;

            // Create one time command pool
            VkResult result = createCommandPool(graphicsQueueFamilyIndex, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, commandPool_);
            if (result != VK_SUCCESS) {
                EMBER_LOG_ERROR("Failed to create one time command pool: {}", std::to_string(result));
                return false;
            }

            // Get graphics queue
            vkGetDeviceQueue(device_, graphicsQueueFamilyIndex, 0, &graphicsQueue_);

            graphicsQueueFamilyIndex_ = graphicsQueueFamilyIndex;
        }

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

    destroyCommandPool(commandPool_);

    if (allocator_ != VK_NULL_HANDLE) {
        vmaDestroyAllocator(allocator_);
        allocator_ = VK_NULL_HANDLE;
    }

    if (device_ != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(device_);
        vkDestroyDevice(device_, nullptr);
        device_ = VK_NULL_HANDLE;
    }

    physicalDevice_ = VK_NULL_HANDLE;
    graphicsQueue_ = VK_NULL_HANDLE;
    graphicsQueueFamilyIndex_ = 0;

    initialized_ = false;

    EMBER_LOG_INFO("VulkanContextVma shutdown complete");
}

void VulkanContextVma::waitIdle() {
    vkDeviceWaitIdle(device_);
}

VkResult VulkanContextVma::createCommandPool(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags,
                                             VkCommandPool& pool) {
    VkCommandPoolCreateInfo info{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    	.flags = flags,
		.queueFamilyIndex = queueFamilyIndex
    };

	return vkCreateCommandPool(device_, &info, nullptr, &pool);
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

bool VulkanContextVma::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& buffer, AllocationHandle& allocation) {
    VkBufferCreateInfo bufferInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.flags = 0,
        .size = size,
        .usage = usage
    };
	
    VmaAllocationCreateInfo allocInfo = {
        .usage = VMA_MEMORY_USAGE_AUTO
    };
    
    VmaAllocation vmaAllocation;
    VkResult result = vmaCreateBuffer(allocator_, &bufferInfo, &allocInfo, &buffer, &vmaAllocation, nullptr);
    if (result != VK_SUCCESS) {
        EMBER_LOG_ERROR("vmaCreateBuffer failed with code: {}", std::to_string(result));
        return false;
    }

    allocation = toAllocationHandle(vmaAllocation);

    return true;
}

void VulkanContextVma::destroyBuffer(VkBuffer buffer, AllocationHandle allocation) {
    vmaDestroyBuffer(allocator_, buffer, toVma(allocation));
}

bool VulkanContextVma::createImage(VkDeviceSize size, VkImageType type, VkImageUsageFlags usage, VkImage& image, AllocationHandle& allocation)
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

    VmaAllocation vmaAllocation;
    VkResult result = vmaCreateImage(allocator_, &bufferInfo, &allocInfo, &image, &vmaAllocation, nullptr);
    if (result == VK_SUCCESS) {
        allocation = toAllocationHandle(vmaAllocation);
        return true;
    }

    return false;
}

bool VulkanContextVma::createShaderModule(std::span<const uint32_t> spirvWords, VkShaderModule& shaderModule)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = spirvWords.size() * sizeof(uint32_t); // Size in bytes
    createInfo.pCode = spirvWords.data();                       // Pointer to SPIR-V array

    VkResult result = vkCreateShaderModule(device_, &createInfo, nullptr, &shaderModule);
    if (result != VK_SUCCESS) {
        EMBER_LOG_ERROR("vkCreateShaderModule failed with code: {}", std::to_string(result));
        return false;
    }

    return true;
}

void VulkanContextVma::destroyShaderModule(VkShaderModule module) {
	vkDestroyShaderModule(device_, module, nullptr);
}

bool VulkanContextVma::createMemoryAllocator() {
    // fetching pointers to Vulkan functions dynamically
    // https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/quick_start.html
    VmaVulkanFunctions vulkanFunctions = {
        .vkGetInstanceProcAddr = &vkGetInstanceProcAddr,
		.vkGetDeviceProcAddr = &vkGetDeviceProcAddr
    };
    
    VmaAllocatorCreateInfo allocatorCreateInfo = {};
    allocatorCreateInfo.physicalDevice = physicalDevice_->handle;
    allocatorCreateInfo.device = device_;
    allocatorCreateInfo.instance = instance_->get();
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
    std::vector< VkDeviceQueueCreateInfo> queueCreateInfos;

    for (auto queueFamily : physicalDevice_->traits.queueFamilies)
    {
        // Queue create info
        
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily.queueFamilyIndex;
        queueCreateInfo.queueCount = queueFamily.queues.size();

        std::vector<float> priorities(queueFamily.queues.size());
        std::ranges::transform(queueFamily.queues, std::back_inserter(priorities), [](const auto q) { return q.priority; });
        queueCreateInfo.pQueuePriorities = priorities.data();

        queueCreateInfos.emplace_back(queueCreateInfo);
    }
    
    // Device features
    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.multiViewport = VK_TRUE;

    // Device create info
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = queueCreateInfos.size();
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    
    std::vector<const char*> enabledExtensions(physicalDevice_->traits.extensions.size());
    std::ranges::transform(physicalDevice_->traits.extensions, enabledExtensions.begin(), [](const auto& ext) { return ext.c_str(); });
	createInfo.enabledExtensionCount = static_cast<uint32_t>(physicalDevice_->traits.extensions.size());
	createInfo.ppEnabledExtensionNames = enabledExtensions.empty() ? nullptr : enabledExtensions.data();

    VkResult result = vkCreateDevice(physicalDevice_->handle, &createInfo, nullptr, &device_);
    if (result != VK_SUCCESS) {
        EMBER_LOG_ERROR("vkCreateDevice failed with code: " + std::to_string(result));
        return false;
    }

    return true;
}

}  // namespace ember::vulkan
