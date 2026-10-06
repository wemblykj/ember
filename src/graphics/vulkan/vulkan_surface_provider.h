#pragma once

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

#include "surface_provider.h"

#include "vulkan_defs.h"
#include "vulkan_instance.h"

namespace ember::graphics::vulkan {

class VulkanSurfaceProvider : public SurfaceProvider {
public:
    // Called before vkCreateInstance — return instance extension names SDL requires.
    virtual ExtensionSet getRequiredInstanceExtensions() const = 0;

	// Called before physical device selection — return device extension names SDL requires.
    virtual ExtensionSet getRequiredDeviceExtensions() const = 0;

    // Called after instance creation. Should create a VkSurfaceKHR and return true on success.
    virtual bool createSurface(const VulkanInstancePtr& instance, VkSurfaceKHR& surface) = 0;

	// Called after instance creation. Should destroy the VkSurfaceKHR created in createSurface.
	virtual void destroySurface(const VulkanInstancePtr& instance, VkSurfaceKHR surface) = 0;
};

using VulkanSurfaceProviderPtr = std::shared_ptr<VulkanSurfaceProvider>;

} // namespace ember::graphics::vulkan