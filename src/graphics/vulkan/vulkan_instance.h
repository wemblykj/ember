#pragma once

#include <memory>

#include <vulkan/vulkan.h>

#include "vulkan_defs.h"

namespace ember::graphics::vulkan {

class VulkanInstance {
public:
	VulkanInstance(VkInstance instance);
	virtual ~VulkanInstance();

	VkInstance get() const { return instance_; }

private:
	VkInstance instance_;
};

using VulkanInstancePtr = std::shared_ptr<VulkanInstance>;

VulkanInstancePtr createVulkanInstance(const ExtensionSet& requiredExtensions = {});

} // namespace ember::graphics::vulkan