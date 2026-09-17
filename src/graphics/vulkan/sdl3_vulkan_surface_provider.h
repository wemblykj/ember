#pragma once

#include "vulkan_surface_provider.h"
#include "window.h" // your platform::Window wrapper exposing SDL_Window* or native handle

namespace ember::graphics {

class SDL3VulkanSurfaceProvider : public vulkan::VulkanSurfaceProvider {
public:
    explicit SDL3VulkanSurfaceProvider(platform::Window* window) : window_(window) {}

    std::vector<const char*> getRequiredInstanceExtensions() const override;
    bool createSurface(VkInstance instance, VkSurfaceKHR &outSurface) override;

private:
    platform::Window* window_; // non-owning
};

} // namespace ember::platform