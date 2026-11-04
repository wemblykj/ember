#pragma once

#include "physical_device_enumerator.h"

#include <vector>

#include "vulkan_instance.h"
#include "physical_device.h"

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