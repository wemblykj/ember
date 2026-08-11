#include "sdl2_window.h"
#include "../../core/logger.h"

namespace ember::platform::sdl2 {

SDL2Window::SDL2Window(const WindowConfig& config)
    : width_(config.width), height_(config.height) {
    using namespace ember::core;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        EMBER_LOG_FATAL(std::string("Failed to initialize SDL2: ") + SDL_GetError());
        return;
    }

    uint32_t flags = SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN;
    if (config.fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN;
    }

    window_ = SDL_CreateWindow(
        config.title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        config.width,
        config.height,
        flags
    );

    if (!window_) {
        EMBER_LOG_FATAL(std::string("Failed to create SDL2 window: ") + SDL_GetError());
        return;
    }

    EMBER_LOG_INFO("SDL2 window created: " + std::to_string(config.width) + "x" + std::to_string(config.height));
}

SDL2Window::~SDL2Window() {
    if (window_) {
        SDL_DestroyWindow(window_);
        SDL_Quit();
    }
}

void SDL2Window::pollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                shouldClose_ = true;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    shouldClose_ = true;
                }
                break;
            default:
                break;
        }
    }
}

void SDL2Window::setTitle(const std::string& title) {
    if (window_) {
        SDL_SetWindowTitle(window_, title.c_str());
    }
}

std::unique_ptr<Window> createWindow(const WindowConfig& config) {
    return std::make_unique<SDL2Window>(config);
}

}  // namespace ember::platform::sdl2
