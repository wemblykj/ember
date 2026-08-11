#include "vulkan_renderer.h"
#include "../../core/logger.h"

namespace ember::graphics::vulkan {

VulkanRenderer::VulkanRenderer(const RendererConfig& config)
    : config_(config) {
}

VulkanRenderer::~VulkanRenderer() {
    shutdown();
}

bool VulkanRenderer::initialize() {
    using namespace ember::core;

    EMBER_LOG_INFO("Initializing Vulkan renderer...");

    context_ = std::make_unique<VulkanContext>(config_);

    EMBER_LOG_INFO("Vulkan renderer initialized");
    return true;
}

void VulkanRenderer::shutdown() {
    using namespace ember::core;

    if (context_) {
        context_->shutdown();
        context_.reset();
    }

    EMBER_LOG_INFO("Vulkan renderer shutdown");
}

void VulkanRenderer::beginFrame() {
    // Implement frame begin logic
}

void VulkanRenderer::endFrame() {
    // Implement frame end logic
}

void VulkanRenderer::present() {
    // Implement present logic
}

void VulkanRenderer::resizeFramebuffer(uint32_t width, uint32_t height) {
    config_.width = width;
    config_.height = height;
}

RendererPtr createRenderer(const RendererConfig& config, platform::Window* window) {
    auto renderer = std::make_unique<VulkanRenderer>(config);
    if (renderer->initialize()) {
        return renderer;
    }
    return nullptr;
}

}  // namespace ember::graphics::vulkan
