#pragma once

#include "ranked_physical_device_selector.h"

namespace ember::graphics::vulkan {

class DefaultPhysicalDeviceSelector : public RankedPhysicalDeviceSelector {
public:
	DefaultPhysicalDeviceSelector& requireExtension(const char* extension) { return *this; }
	DefaultPhysicalDeviceSelector& supportsSurface(VkSurfaceKHR surface) { return *this; }

private:
	int evaluateCandidate(const DeviceSelectionResult& candidate) override;
};

using DefaultPhysicalDeviceSelectorPtr = std::unique_ptr<DefaultPhysicalDeviceSelector>;

DefaultPhysicalDeviceSelectorPtr createDefaultPhysicalDeviceSelector();
} // namespace ember::graphics::vulkan

