#pragma once

#include "../renderer.h"
#include "../../platform/window.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include <cstdint>

namespace ember::graphics::vulkan {

class VulkanContext {
public:
    explicit VulkanContext(const RendererConfig& config);
    ~VulkanContext();

    bool initialize(platform::Window* window);
    void shutdown();

    VkInstance getInstance() const { return instance_; }
    VkPhysicalDevice getPhysicalDevice() const { return physicalDevice_; }
    VkDevice getDevice() const { return device_; }
    VkQueue getGraphicsQueue() const { return graphicsQueue_; }
    VkSurfaceKHR getSurface() const { return surface_; }
    uint32_t getGraphicsQueueFamily() const { return graphicsQueueFamily_; }

private:
    bool createInstance();
    bool selectPhysicalDevice();
    bool createLogicalDevice();
    bool createSurface(platform::Window* window);

    RendererConfig config_;
    VkInstance instance_ = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
    VkDevice device_ = VK_NULL_HANDLE;
    VkQueue graphicsQueue_ = VK_NULL_HANDLE;
    VkSurfaceKHR surface_ = VK_NULL_HANDLE;
    uint32_t graphicsQueueFamily_ = 0;
};

}  // namespace ember::graphics::vulkan
