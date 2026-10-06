#pragma once

#include "physical_device_selector.h"

#include <optional>

namespace ember::graphics::vulkan {

class AnyPhysicalDeviceSelector : public PhysicalDeviceSelector {

private:
	std::optional<PhysicalDevicePtr> select(const PhysicalDevices& candidates) override;
};

PhysicalDeviceSelectorPtr createAnyPhysicalDeviceSelector();

} // namespace ember::graphics::vulkan