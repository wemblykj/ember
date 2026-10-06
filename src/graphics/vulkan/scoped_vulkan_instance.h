#pragma once

#include "vulkan_instance.h"

#include <vulkan/vulkan.h>

namespace ember::graphics::vulkan {

class ScopedVulkanInstance : public VulkanInstance {
public:
    ScopedVulkanInstance(VkInstance instance) : VulkanInstance(instance) {}
	~ScopedVulkanInstance();
};

} // namespace ember::graphics::vulkan