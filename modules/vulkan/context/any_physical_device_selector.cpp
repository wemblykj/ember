#include "any_physical_device_selector.h"

namespace ember::vulkan {

	std::optional<PhysicalDevicePtr> select(const PhysicalDevices& candidates)
{
	if (candidates.empty()) {
		return std::nullopt; // No devices found
	}

	return candidates.front();
}

} // namespace ember::vulkan