#pragma once

#include <memory>

#include <vulkan/vulkan.hpp>

#include <vulkan/vulkan_defs.hpp>

namespace ember::vulkan {

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

} // namespace ember::vulkan