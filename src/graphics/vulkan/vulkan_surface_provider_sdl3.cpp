#include "vulkan_surface_provider_sdl3.h"

#include <vector>

#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL_video.h>

#include "vulkan_instance.h"

namespace ember::graphics::vulkan {

    ExtensionSet VulkanSurfaceProviderSDL3::getRequiredInstanceExtensions() const {
    ExtensionSet out;

    if (!window_) return out;

	unsigned count = 0;
    auto extensions = SDL_Vulkan_GetInstanceExtensions(&count);
    for (unsigned i = 0; i < count; ++i) {
        out.insert(extensions[i]);
	}

    return out;
}

ExtensionSet VulkanSurfaceProviderSDL3::getRequiredDeviceExtensions() const {
    // SDL3 does not provide device extensions, but this is a placeholder for future use if needed.
	//SDL_Vulkan_GetDeviceExtensions(nullptr, nullptr, nullptr, nullptr); 
    return {};
}

bool VulkanSurfaceProviderSDL3::createSurface(const VulkanInstancePtr& instance, VkSurfaceKHR& surface) {
    if (!window_) return false;

    auto sdlWindow = reinterpret_cast<SDL_Window*>(window_->getNativeHandle());

    if (!SDL_Vulkan_CreateSurface(sdlWindow, instance->get(), nullptr, &surface)) {
        return false;
    }

    return true;
}

void VulkanSurfaceProviderSDL3::destroySurface(const VulkanInstancePtr& instance, VkSurfaceKHR surface) {
    auto sdlWindow = reinterpret_cast<SDL_Window*>(window_->getNativeHandle());

    SDL_Vulkan_DestroySurface(instance->get(), surface, nullptr);
}

SurfaceProviderPtr createSDLSurfaceProvider(platform::Window* window)
{
    auto provider = std::make_unique<VulkanSurfaceProviderSDL3>(window);

    return provider;
}

} // namespace ember::graphics::vulkan