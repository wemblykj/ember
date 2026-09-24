#include "application.h"

#include "platform/sdl3/sdl3_window.h"

#include "graphics/vulkan/vulkan_renderer.h"
#include "graphics/vulkan/vulkan_surface_provider_sdl3.h"

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
    window_ = platform::sdl3::createWindow(windowConfig);
    if (!window_) {
        EMBER_LOG_ERROR("Failed to create window");
        return false;
    }

    surfaceProvider_ = graphics::vulkan::createSDLSurfaceProvider(window_.get());

    // Create renderer
    renderer_ = graphics::vulkan::createRenderer(rendererConfig, surfaceProvider_);
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

    auto lastTime = std::chrono::high_resolution_clock::now();

    while (isRunning_ && !window_->shouldClose()) {
        // Calculate delta time
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(
            currentTime - lastTime
        ).count();
        lastTime = currentTime;

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
