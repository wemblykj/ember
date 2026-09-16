#include "sdl3_window.h"
#include <SDL3/SDL_vulkan.h>

namespace ember::platform::sdl3 {

SDL3Window::SDL3Window(const WindowConfig& config)
    : width_(config.width), height_(config.height) {
    using namespace ember::core;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        EMBER_LOG_FATAL("Failed to initialize SDL3: {}", SDL_GetError());
        return;
    }

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, config.title.c_str());
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_CENTERED);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_CENTERED);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, config.width);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, config.height);
    
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN, hasFeature(config.features, WindowFeature::Fullscreen));

    switch (config.api) {
    case graphics::GraphicsAPI::Vulkan:
        SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_VULKAN_BOOLEAN, true);
        break;
    case graphics::GraphicsAPI::OpenGL:
        SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN, true);
        break;
    case graphics::GraphicsAPI::DirectX12:
        // SDL doesn't have a DirectX flag; backends may need native handle
        break;
	case graphics::GraphicsAPI::Metal:
        SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_METAL_BOOLEAN, true);
        break;
    case graphics::GraphicsAPI::Default:
    default:
        // optionally prefer Vulkan when available: handled in higher-level init
        break;
    }
    
    window_ = SDL_CreateWindowWithProperties(props);

    SDL_DestroyProperties(props);

    if (!window_) {
        EMBER_LOG_FATAL("Failed to create SDL3 window: {}", SDL_GetError());
        return;
    }

    EMBER_LOG_INFO("SDL3 window created: {}x{}", config.width, config.height);
}

SDL3Window::~SDL3Window() {
    if (window_) {
        SDL_DestroyWindow(window_);
        SDL_Quit();
    }
}

void SDL3Window::pollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                shouldClose_ = true;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (event.key.key == SDLK_ESCAPE) {
                    shouldClose_ = true;
                }
                break;
            default:
                break;
        }
    }
}

void SDL3Window::setTitle(const std::string& title) {
    if (window_) {
        SDL_SetWindowTitle(window_, title.c_str());
    }
}

std::unique_ptr<Window> createWindow(const WindowConfig& config) {
    return std::make_unique<SDL3Window>(config);
}

}  // namespace ember::platform::sdl3