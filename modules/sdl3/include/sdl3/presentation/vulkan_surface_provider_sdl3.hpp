#pragma once

#include <vulkan/presentation/vulkan_surface_provider.hpp>

#include <memory>

#include <vulkan/vulkan.h>

#include <graphics/presentation/surface_provider.hpp>

#include <platform/window.hpp>

#include <vulkan/vulkan_defs.hpp>
#include <vulkan/vulkan_instance.hpp>

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