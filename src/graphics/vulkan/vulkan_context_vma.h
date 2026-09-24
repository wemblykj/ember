#pragma once

#include "vulkan_context.h"

#include <vector>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "vulkan_surface_provider.h"

namespace ember::graphics::vulkan {

/**
* @brief Concrete implementation of VulkanContext using Vulkan Memory Allocator (VMA) for memory management.
*/
class VulkanContextVma : public VulkanContext {

public:
    VulkanContextVma();
    ~VulkanContextVma();

    VkInstance createInstance(const std::vector<const char*>& extensions) override;
    VkPhysicalDevice createDevice(DeviceSelectorCallback selector = DeviceSelector::any) override;
    void destroy() override;

    void waitIdle() override;

    VkInstance getInstance() const override { return instance_; }
    VkPhysicalDevice getPhysicalDevice() const override { return physicalDevice_; }
    VkDevice getDevice() const override { return device_; }
    VkQueue getGraphicsQueue() const override { return graphicsQueue_; }
    uint32_t getGraphicsQueueFamily() const override { return graphicsQueueFamily_; }

    VkResult createCommandPool(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags, VkCommandPool* outPool) override ;
    void destroyCommandPool(VkCommandPool pool) override;
    VkResult allocateCommandBuffers(VkCommandPool pool, VkCommandBufferLevel level, uint32_t count, VkCommandBuffer* outBuffers) override;
    void freeCommandBuffers(VkCommandPool pool, uint32_t count, const VkCommandBuffer* buffers) override;
    VkCommandBuffer beginOneTimeCommands() override;
    void endOneTimeCommands(VkCommandBuffer cmd) override;
    VkResult createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer* outBuffer, AllocationHandle* outMemory) override;
    void destroyBuffer(VkBuffer buffer, AllocationHandle memory) override;
    VkResult createImage(VkDeviceSize size, VkImageType type, VkImageUsageFlags usage, VkImage* outImage, AllocationHandle* outMemory) override;

private:
    VkPipelineCache getPipelineCache() const { return pipelineCache_; }

private:
    bool createInstanceImpl(std::vector<const char*> extensions);
    bool selectPhysicalDevice(DeviceSelectorCallback selector);
    bool createLogicalDevice();
	bool createMemoryAllocator();

    static VmaAllocation toVma(AllocationHandle handle) {
        assert(handle.backend == AllocationHandle::Backend::Vma); 
        return reinterpret_cast<VmaAllocation>(handle.handle); 
    }

	static AllocationHandle toAllocationHandle(VmaAllocation allocation) {
        return AllocationHandle{ AllocationHandle::Backend::Vma, reinterpret_cast<uintptr_t>(allocation) };
    }

    bool initialized_ = false;
    VkInstance instance_ = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
    VkDevice device_ = VK_NULL_HANDLE;
    VkQueue graphicsQueue_ = VK_NULL_HANDLE;
    uint32_t graphicsQueueFamily_ = 0;
    VkPipelineCache pipelineCache_ = VK_NULL_HANDLE;
    VmaAllocator allocator_ = VK_NULL_HANDLE;
	VkCommandPool commandPool_ = VK_NULL_HANDLE;
};

}  // namespace ember::graphics::vulkan
