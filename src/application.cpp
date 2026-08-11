#include "application.h"
#include "platform/sdl2/sdl2_window.h"
#include "graphics/vulkan/vulkan_renderer.h"

namespace ember {

Application::Application() {
}

Application::~Application() {
    shutdown();
}

bool Application::initialize(const platform::WindowConfig& windowConfig,
                            const graphics::RendererConfig& rendererConfig) {
    using namespace ember::core;

    EMBER_LOG_INFO("Initializing Ember Application...");

    // Create window
    window_ = platform::sdl2::createWindow(windowConfig);
    if (!window_) {
        EMBER_LOG_ERROR("Failed to create window");
        return false;
    }

    // Create renderer
    renderer_ = graphics::vulkan::createRenderer(rendererConfig, window_.get());
    if (!renderer_) {
        EMBER_LOG_ERROR("Failed to create renderer");
        return false;
    }

    isRunning_ = true;

    EMBER_LOG_INFO("Ember Application initialized successfully");
    return true;
}

void Application::run() {
    using namespace ember::core;

    EMBER_LOG_INFO("Starting application main loop...");

    while (isRunning_ && !window_->shouldClose()) {
        window_->pollEvents();

        renderer_->beginFrame();
        // Application logic goes here
        renderer_->endFrame();
        renderer_->present();
    }

    EMBER_LOG_INFO("Application main loop ended");
}

void Application::shutdown() {
    using namespace ember::core;

    isRunning_ = false;

    if (renderer_) {
        renderer_->shutdown();
        renderer_.reset();
    }

    if (window_) {
        window_.reset();
    }

    EMBER_LOG_INFO("Ember Application shutdown complete");
}

}  // namespace ember
