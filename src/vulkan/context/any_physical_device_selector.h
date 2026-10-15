#pragma once

#include "physical_device_selector.h"

#include <optional>

namespace ember::vulkan {

class AnyPhysicalDeviceSelector : public PhysicalDeviceSelector {

private:
	std::optional<PhysicalDevicePtr> select(const PhysicalDevices& candidates) override;
};

} // namespace ember::vulkan