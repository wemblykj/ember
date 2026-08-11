#pragma once

#include <string>
#include <memory>
#include <utility>

namespace ember::platform {

/**
 * Abstract window interface.
 * Implementations (SDL2, GLFW, native, etc.) inherit from this.
 */
class Window {
public:
    virtual ~Window() = default;

    virtual bool shouldClose() const = 0;
    virtual void pollEvents() = 0;
    virtual std::pair<uint32_t, uint32_t> getSize() const = 0;
    virtual void setTitle(const std::string& title) = 0;

    // Platform-specific native window handle (void* to avoid exposing platform headers)
    virtual void* getNativeHandle() = 0;
};

using WindowPtr = std::unique_ptr<Window>;

/**
 * Window creation configuration
 */
struct WindowConfig {
    std::string title = "Ember Engine";
    uint32_t width = 1280;
    uint32_t height = 720;
    bool vsync = true;
    bool fullscreen = false;
};

}  // namespace ember::platform
