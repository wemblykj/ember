#pragma once

#include <memory>

#include <platform/window.hpp>
#include <resources/resource_file_provider.hpp>

#include <graphics/renderer.hpp>

namespace ember::application {

/**
 * Main application class that manages window and renderer lifecycle
 */
class Application {
public:
    Application();
    ~Application();

    bool initialize(const platform::WindowConfig& windowConfig,
                   const graphics::RendererConfig& rendererConfig);
    void run();
    void shutdown();

    platform::Window* getWindow() const { return window_.get(); }
    graphics::Renderer* getRenderer() const { return renderer_.get(); }

private:
    resources::ResourceFileProviderPtr fileProvider_;
    platform::WindowPtr window_;
	graphics::SurfaceProviderPtr surfaceProvider_;
    graphics::RendererPtr renderer_;

    bool isRunning_ = false;
};

}  // namespace ember::application
