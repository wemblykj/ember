#pragma once

#include "vulkan_surface_provider.h"

#include <memory>

#include <vulkan/vulkan.h>

#include <surface_provider.h>

#include "window.h"
#include "vulkan_defs.h"
#include "context/vulkan_instance.h"

namespace ember::vulkan {

class VulkanSurfaceProviderSDL3 : public vulkan::VulkanSurfaceProvider {
public:
    explicit VulkanSurfaceProviderSDL3(platform::Window* window) : window_(window) {}

    ExtensionSet getRequiredInstanceExtensions() const override;
    ExtensionSet getRequiredDeviceExtensions() const override;
    bool createSurface(const VulkanInstancePtr& instance, VkSurfaceKHR &outSurface) override;
    void destroySurface(const VulkanInstancePtr& instance, VkSurfaceKHR surface) override;

private:
    platform::Window* window_; // non-owning
};

/**
* Factory function for creating Vulkan renderers
*/
graphics::SurfaceProviderPtr createSDLSurfaceProvider(platform::Window* window);

} // namespace ember::vulkan