#pragma once

#include <memory>

#include "vulkan_surface_provider.h"

#include "surface_provider.h"
#include "window.h" // your platform::Window wrapper exposing SDL_Window* or native handle

namespace ember::graphics::vulkan {

class VulkanSurfaceProviderSDL3 : public vulkan::VulkanSurfaceProvider {
public:
    explicit VulkanSurfaceProviderSDL3(platform::Window* window) : window_(window) {}

    std::vector<const char*> getRequiredInstanceExtensions() const override;
    bool createSurface(VkInstance instance, VkSurfaceKHR &outSurface) override;

private:
    platform::Window* window_; // non-owning
};

/**
* Factory function for creating Vulkan renderers
*/
SurfaceProviderPtr createSDLSurfaceProvider(platform::Window* window);

} // namespace ember::graphics::vulkan