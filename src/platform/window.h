#pragma once

#include <string>
#include <memory>
#include <optional>
#include <utility>
#include <variant>

#include <graphics_api.h>

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
* Optional per-API configuration. Only fill what you need.
* Backends will inspect the corresponding variant alternative.
*/
struct VulkanOptions {
    bool enableValidation = false;
    std::optional<uint32_t> requestedInstanceVersion; // e.g. VK_API_VERSION_1_2
    // add more Vulkan-specific options here
};

struct OpenGLOptions {
    int major = 4;
    int minor = 5;
    bool coreProfile = true;
    // add more GL-specific options here
};

struct DirectXOptions {
    // example placeholder for DirectX request
    uint32_t featureLevel = 0; // 0 = default
};

using APIOptions = std::variant<std::monostate, VulkanOptions, OpenGLOptions, DirectXOptions>;

/**
* Orthogonal feature hints that aren't exclusive to one API.
* Use a simple bitmask for composable hints.
*/
enum class WindowFeature : uint32_t {
    None = 0,
    VSync = 1u << 0,
    Resizable = 1u << 1,
    Fullscreen = 1u << 2,
    HighDPI = 1u << 3,
    SRGB = 1u << 4
};

inline WindowFeature operator|(WindowFeature a, WindowFeature b) {
    return static_cast<WindowFeature>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}
inline bool hasFeature(WindowFeature mask, WindowFeature flag) {
    return (static_cast<uint32_t>(mask) & static_cast<uint32_t>(flag)) != 0;
}

/**
 * Window creation configuration
 */
struct WindowConfig {
    std::string title = "Ember Engine";
    uint32_t width = 1280;
    uint32_t height = 720;

    // Composable hints
    WindowFeature features = WindowFeature::VSync | WindowFeature::Resizable;

    // Primary requested API (Default = let backend choose)
    graphics::GraphicsAPI api = graphics::GraphicsAPI::Default;

    // Optional per-API fine tuning
    APIOptions apiOptions;
};

}  // namespace ember::platform
