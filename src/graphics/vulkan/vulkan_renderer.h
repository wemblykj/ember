#pragma once

#include "../renderer.h"

#include <memory>
#include <string>
#include <vector>

#include "vulkan_context_vma.h"
#include "resource_records.h"
#include "vulkan_resource_cache_vma.h"

namespace ember::graphics::vulkan {

/**
 * @brief Vulkan renderer implementation
 */
class VulkanRenderer : public Renderer {
public:
    explicit VulkanRenderer(const RendererConfig& config, std::shared_ptr<VulkanContextVma> context, std::shared_ptr<VulkanResourceCacheVma> resourceCache);
    ~VulkanRenderer() override;
    
    bool initialize(platform::SurfaceProvider* provider) override;
    void shutdown() override;

    ResourceCache& getResourceCache() override { return *resourceCache_; }

    void beginFrame() override;
    void submitPass(const RendererPass& pass) override;
    void endFrame() override;
    void present() override;
    void resizeFramebuffer(uint32_t width, uint32_t height) override;

private:
    bool createFrameResources();
    void destroyFrameResources();

private:
    struct FrameContext {
        VkCommandPool commandPool = VK_NULL_HANDLE;
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        VkFence inFlightFence = VK_NULL_HANDLE;
        VkSemaphore imageAvailable = VK_NULL_HANDLE;
        VkSemaphore renderFinished = VK_NULL_HANDLE;
	};

    std::shared_ptr<VulkanContextVma> context_;
    std::shared_ptr<VulkanResourceCacheVma> resourceCache_;

    std::vector<FrameContext> frames_;
    uint32_t currentFrameIndex_ = 0;
    uint32_t framesInFlight_ = 2;

    RendererConfig config_;
};

/**
 * Factory function for creating Vulkan renderers
 */
RendererPtr createRenderer(const RendererConfig& config, platform::SurfaceProvider* provider);

}  // namespace ember::graphics::vulkan
