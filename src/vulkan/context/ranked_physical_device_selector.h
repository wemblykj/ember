#pragma once

#include "physical_device_selector.h"

namespace ember::graphics::vulkan {

class RankedPhysicalDeviceSelector : public PhysicalDeviceSelector {

public:
	std::optional<DeviceSelectionResult> select(VkInstance instance) override;

	/**
	 * Enumerate candidate physical devices for the given instance.
	 * @param instance Vulkan instance (already created by context).
	 * @return A vector of candidate device selection results.
	 */
	std::vector<DeviceSelectionResult> candidates(VkInstance instance);

protected:
	using RankedCandidate = std::tuple<int, DeviceSelectionResult>; // score, result

	virtual int evaluateCandidate(const DeviceSelectionResult& candidate) = 0;

private:
	std::vector<RankedCandidate> rankedCandidates(VkInstance instance);
};

} // namespace ember::graphics::vulkan

