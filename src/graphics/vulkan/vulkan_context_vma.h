#pragma once

#include "../renderer.h"

#include <vector>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "vulkan_surface_provider.h"

namespace ember::graphics::vulkan {

class VulkanContextVma {
public:
    explicit VulkanContextVma(const RendererConfig& config);
    ~VulkanContextVma();

    bool initialize(VulkanSurfaceProvider* provider);
    void shutdown();

    VkInstance getInstance() const { return instance_; }
    VkPhysicalDevice getPhysicalDevice() const { return physicalDevice_; }
    VkDevice getDevice() const { return device_; }
    VkQueue getGraphicsQueue() const { return graphicsQueue_; }
    VkSurfaceKHR getSurface() const { return surface_; }
    uint32_t getGraphicsQueueFamily() const { return graphicsQueueFamily_; }

    // Command pool / command buffer helpers (context allocates/destroys, renderer owns pools)
    VkResult createCommandPool(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags, VkCommandPool* outPool);
    void destroyCommandPool(VkCommandPool pool);

    VkResult allocateCommandBuffers(VkCommandPool pool, VkCommandBufferLevel level, uint32_t count, VkCommandBuffer* outBuffers);
    void freeCommandBuffers(VkCommandPool pool, uint32_t count, const VkCommandBuffer* buffers);

    // One-shot convenience: begin/end single-use command buffer and submit with optional fence
    VkCommandBuffer beginOneTimeCommands();
    void endOneTimeCommands(VkCommandBuffer cmd);

    // Helpers for buffer/image creation, shader module, pipeline cache etc.
    VkResult createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer* outBuffer, VmaAllocation* outMemory);
    void destroyBuffer(VkBuffer buffer, VkDeviceMemory memory);

    VkPipelineCache getPipelineCache() const { return pipelineCache_; }

private:
    bool createInstance(std::vector<const char*> extensions);
    bool selectPhysicalDevice();
    bool createLogicalDevice();
    std::vector<const char*> getRequiredExtensions();
    bool createSurface(VulkanSurfaceProvider* surfaceProvider);
	bool createMemoryAllocator();
    bool initialized_ = false;
    RendererConfig config_;
    VkInstance instance_ = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
    VkDevice device_ = VK_NULL_HANDLE;
    VkQueue graphicsQueue_ = VK_NULL_HANDLE;
    VkSurfaceKHR surface_ = VK_NULL_HANDLE;
    uint32_t graphicsQueueFamily_ = 0;
    VkPipelineCache pipelineCache_ = VK_NULL_HANDLE;
    VmaAllocator allocator_ = VK_NULL_HANDLE;
};

}  // namespace ember::graphics::vulkan
