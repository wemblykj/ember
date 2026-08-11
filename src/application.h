#pragma once

#include "platform/window.h"
#include "graphics/renderer.h"
#include <memory>

namespace ember {

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
    platform::WindowPtr window_;
    graphics::RendererPtr renderer_;
    bool isRunning_ = false;
};

}  // namespace ember
