#pragma once

#include "physical_device_enumerator.h"

#include <vector>

#include "vulkan_instance.h"
#include "physical_device.h"

namespace ember::vulkan {

class DefaultPhysicalDeviceEnumerator : public PhysicalDeviceEnumerator {
public:
	std::vector<PhysicalDevicePtr> enumerate(const VulkanInstancePtr& instance) override
};

} // namespace ember::vulkan