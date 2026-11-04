#ifdef EMBER_OPENXR_ENABLED

#include "openxr_renderer.h"
#include "core/logger.h"

namespace ember::graphics::openxr {

OpenXRRenderer::OpenXRRenderer(const RendererConfig& config)
    : config_(config) {
}

OpenXRRenderer::~OpenXRRenderer() {
    shutdown();
}

bool OpenXRRenderer::initialize() {
    if (initialized_) {
        return true;
    }

    EMBER_LOG_INFO("Initializing OpenXR renderer...");

    // Create OpenXR context
    context_ = std::make_unique<OpenXRContext>();
    if (!context_->initialize(config_.xrAppName)) {
        EMBER_LOG_ERROR("Failed to initialize OpenXR context");
        return false;
    }

    // Create OpenXR session
    session_ = std::make_unique<OpenXRSession>(context_.get());
    if (!session_->initialize()) {
        EMBER_LOG_ERROR("Failed to initialize OpenXR session");
        return false;
    }

    initialized_ = true;
    EMBER_LOG_INFO("OpenXR renderer initialized successfully");
    return true;
}

void OpenXRRenderer::shutdown() {
    if (!initialized_) {
        return;
    }

    if (session_) {
        session_->shutdown();
        session_.reset();
    }

    if (context_) {
        context_->shutdown();
        context_.reset();
    }

    initialized_ = false;
    EMBER_LOG_INFO("OpenXR renderer shutdown complete");
}

bool OpenXRRenderer::beginVRFrame(uint32_t& viewCount, EyeView* views) {
    if (!initialized_ || !session_) {
        return false;
    }

    // In a full implementation, this would:
    // 1. Wait for the frame (xrWaitFrame)
    // 2. Get predicted views for each eye
    // 3. Populate the views array with projection and view matrices
    // 4. Begin frame submission (xrBeginFrame)

    if (!session_->waitFrame() || !session_->beginFrame()) {
        return false;
    }

    viewCount = session_->getViewCount();
    // TODO: Copy view data from session to views array

    EMBER_LOG_DEBUG("OpenXR VR frame begun");
    return true;
}

void OpenXRRenderer::endVRFrame() {
    if (!initialized_ || !session_) {
        return;
    }

    // In a full implementation, this would:
    // 1. Submit the composed swapchain layers
    // 2. End the frame (xrEndFrame)

    session_->endFrame();

    EMBER_LOG_DEBUG("OpenXR VR frame ended");
}

void OpenXRRenderer::resizeFramebuffer(uint32_t width, uint32_t height) {
    config_.width = width;
    config_.height = height;
}

RendererPtr createOpenXRRenderer(const RendererConfig& config) {
    auto renderer = std::make_unique<OpenXRRenderer>(config);
    if (!renderer->initialize()) {
        return nullptr;
    }
    return renderer;
}

}  // namespace ember::graphics::openxr

#endif // EMBER_OPENXR_ENABLED
