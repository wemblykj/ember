#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include <memory>

#include "surface_provider.h"

namespace ember::graphics::vulkan {

class VulkanSurfaceProvider : public SurfaceProvider {
public:
    // Called before vkCreateInstance — return instance extension names SDL requires.
    virtual std::vector<const char*> getRequiredInstanceExtensions() const = 0;

    // Called after instance creation. Should create a VkSurfaceKHR and return true on success.
    virtual bool createSurface(VkInstance instance, VkSurfaceKHR &outSurface) = 0;
};

using VulkanSurfaceProviderPtr = std::shared_ptr<VulkanSurfaceProvider>;

} // namespace ember::graphics::vulkan