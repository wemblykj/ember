#pragma once

#include "physical_device_selector.h"

namespace ember::graphics::vulkan {

class AnyPhysicalDeviceSelector : public PhysicalDeviceSelector {

private:
	std::optional<DeviceSelectionResult> select(VkInstance instance) override;
};

PhysicalDeviceSelectorPtr createAnyPhysicalDeviceSelector();
} // namespace ember::graphics::vulkan