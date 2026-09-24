//#include "sample_scene.h"
#include "application.h"
#include "core/logger.h"
#include <chrono>

int main(int argc, char* argv[]) {
    using namespace ember;
    using namespace ember::core;

    EMBER_LOG_INFO("Starting Ember Engine Sample...");

    // Create and initialize application
    Application app;

    platform::WindowConfig windowConfig{
        .title = "Ember Engine - Sample",
        .width = 1280,
        .height = 720,
		.features = platform::WindowFeature::VSync | platform::WindowFeature::Resizable,
        .api = graphics::GraphicsAPI::Vulkan
    };

    graphics::RendererConfig rendererConfig{
        .width = 1280,
        .height = 720,
        .enableValidation = true,
        .vsync = true,
        .api = graphics::GraphicsAPI::Vulkan
    };

    if (!app.initialize(windowConfig, rendererConfig)) {
        EMBER_LOG_ERROR("Failed to initialize application");
        return 1;
    }

    app.run();

    // Cleanup
    //scene.shutdown();
    app.shutdown();

    EMBER_LOG_INFO("Ember Engine Sample shutdown");
    return 0;
}
