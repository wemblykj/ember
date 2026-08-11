#include "sample_scene.h"
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
        .vsync = true,
        .fullscreen = false
    };

    graphics::RendererConfig rendererConfig{
        .width = 1280,
        .height = 720,
        .enableValidation = true,
        .vsync = true
    };

    if (!app.initialize(windowConfig, rendererConfig)) {
        EMBER_LOG_ERROR("Failed to initialize application");
        return 1;
    }

    // Create sample scene
    samples::SampleScene scene;
    if (!scene.initialize(app.getRenderer())) {
        EMBER_LOG_ERROR("Failed to initialize sample scene");
        app.shutdown();
        return 1;
    }

    // Main loop with delta time calculation
    auto lastTime = std::chrono::high_resolution_clock::now();
    
    while (app.getWindow() && !app.getWindow()->shouldClose()) {
        // Calculate delta time
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(
            currentTime - lastTime
        ).count();
        lastTime = currentTime;

        // Poll window events
        app.getWindow()->pollEvents();

        // Update scene
        scene.update(deltaTime);

        // Render frame
        scene.render();
    }

    // Cleanup
    scene.shutdown();
    app.shutdown();

    EMBER_LOG_INFO("Ember Engine Sample shutdown");
    return 0;
}
