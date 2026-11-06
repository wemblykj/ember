#pragma once

#include <vulkan/vulkan_instance.hpp>

#include <vulkan/vulkan.hpp>

namespace ember::vulkan {

class ScopedVulkanInstance : public VulkanInstance {
public:
    ScopedVulkanInstance(VkInstance instance) : VulkanInstance(instance) {}
	~ScopedVulkanInstance();
};

} // namespace ember::vulkan