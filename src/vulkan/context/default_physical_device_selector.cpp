#include "default_physical_device_selector.h"

namespace ember::graphics::vulkan {

int DefaultPhysicalDeviceSelector::evaluateCandidate(const DeviceSelectionResult& candidate)
{
	return 0;
}

DefaultPhysicalDeviceSelectorPtr createDefaultPhysicalDeviceSelector()
{
	return std::make_unique<DefaultPhysicalDeviceSelector>();
}

}
