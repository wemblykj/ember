#pragma once

#include "physical_device_enumerator.hpp"

#include <vector>

#include <vulkan/vulkan_instance.hpp>
#include <vulkan/context/physical_device.hpp>

namespace ember::vulkan {

class DefaultPhysicalDeviceEnumerator : public PhysicalDeviceEnumerator {
public:
	DefaultPhysicalDeviceEnumerator(const VulkanInstancePtr& instance);
	std::vector<PhysicalDevicePtr> enumerate() override;

private:
	PhysicalDevicePtr buiildPhysicalDevice(VkPhysicalDevice device);

	VulkanInstancePtr instance_;
};

} // namespace ember::vulkan