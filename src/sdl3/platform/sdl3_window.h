#pragma once

#include <window.h>

#include <cstdint>
#include <memory>

#include <SDL3/SDL.h>

namespace ember::platform::sdl3 {

class SDL3Window : public Window {
public:
    explicit SDL3Window(const WindowConfig& config);
    ~SDL3Window() override;

    bool shouldClose() const override { return shouldClose_; }
    void pollEvents() override;
    std::pair<uint32_t, uint32_t> getSize() const override { return {width_, height_}; }
    void setTitle(const std::string& title) override;
    void* getNativeHandle() override { return window_; }

private:
    SDL_Window* window_ = nullptr;
    uint32_t width_;
    uint32_t height_;
    bool shouldClose_ = false;
};

/**
 * Factory function for creating SDL3 windows
 */
WindowPtr createWindow(const WindowConfig& config);

}  // namespace ember::platform::sdl3
