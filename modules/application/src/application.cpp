#include <application/application.hpp>

#include <core/logger.hpp>

#include <resources/canned_resources.hpp>

#include <sdl3/platform/sdl3_window.hpp>
#include <sdl3/presentation/vulkan_surface_provider_sdl3.hpp>

#include <vulkan/vulkan_renderer.hpp>
#include <vulkan/resources/default_vulkan_resource_file_provider.hpp>

#include <application/native_file_provider.hpp>

namespace ember::application {

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

    //fileProvider_ = std::make_shared<NativeFileProvider>("./resources");
	fileProvider_ = std::make_shared<vulkan::resources::DefaultVulkanResourceFileProvider>();

    surfaceProvider_ = vulkan::createSDLSurfaceProvider(window_.get());

    // Create renderer
    renderer_ = vulkan::createRenderer(rendererConfig, fileProvider_, surfaceProvider_);
    if (!renderer_) {
        EMBER_LOG_ERROR("Failed to create renderer");
        return false;
    }

    auto& resourceCache = renderer_->getResourceCache();

	resourceCache.assertTechnique(resources::CannedTechnique::Opaque);
    resourceCache.assertTechnique(resources::CannedTechnique::Transparent);
    resourceCache.assertMaterial(resources::CannedMaterial::Undefined);
    resourceCache.assertGeometry(resources::CannedGeometry::UnitQuad);

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

}  // namespace ember::application
