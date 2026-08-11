#pragma once

#include "../renderer.h"
#include "vulkan_context.h"
#include <memory>

namespace ember::graphics::vulkan {

/**
 * Vulkan renderer implementation
 */
class VulkanRenderer : public Renderer {
public:
    explicit VulkanRenderer(const RendererConfig& config);
    ~VulkanRenderer() override;

    bool initialize() override;
    void shutdown() override;
    void beginFrame() override;
    void endFrame() override;
    void present() override;
    void resizeFramebuffer(uint32_t width, uint32_t height) override;

private:
    RendererConfig config_;
    std::unique_ptr<VulkanContext> context_;
};

/**
 * Factory function for creating Vulkan renderers
 */
RendererPtr createRenderer(const RendererConfig& config, platform::Window* window);

}  // namespace ember::graphics::vulkan
