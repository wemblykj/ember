#include "sdl3_vulkan_surface_provider.h"

#include <vector>

#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL_video.h>

namespace ember::graphics {

std::vector<const char*> SDL3VulkanSurfaceProvider::getRequiredInstanceExtensions() const {
    std::vector<const char*> out;

    if (!window_) return out;

	unsigned count = 0;
    auto extensions = SDL_Vulkan_GetInstanceExtensions(&count);
    for (unsigned i = 0; i < count; ++i) {
        out.push_back(extensions[i]);
	}

    return out;
}

bool SDL3VulkanSurfaceProvider::createSurface(VkInstance instance, VkSurfaceKHR &outSurface) {
    if (!window_) return false;

    auto sdlWindow = reinterpret_cast<SDL_Window*>(window_->getNativeHandle());

    if (!SDL_Vulkan_CreateSurface(sdlWindow, instance, nullptr, &outSurface)) {
        return false;
    }

    return true;
}

} // namespace ember::platform