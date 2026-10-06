#include "any_physical_device_selector.h"

namespace ember::graphics::vulkan {

	std::optional<PhysicalDevicePtr> select(const PhysicalDevices& candidates)
{
	if (candidates.empty()) {
		return std::nullopt; // No devices found
	}

	return candidates.front();
}

PhysicalDeviceSelectorPtr createAnyPhysicalDeviceSelector()
{
	return std::move(std::make_unique<AnyPhysicalDeviceSelector>());
}

} // namespace ember::graphics::vulkan