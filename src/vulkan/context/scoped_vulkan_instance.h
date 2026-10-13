#pragma once

#include "vulkan_instance.h"

#include <vulkan/vulkan.h>

namespace ember::vulkan {

class ScopedVulkanInstance : public VulkanInstance {
public:
    ScopedVulkanInstance(VkInstance instance) : VulkanInstance(instance) {}
	~ScopedVulkanInstance();
};

} // namespace ember::vulkan