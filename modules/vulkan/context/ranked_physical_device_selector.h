#pragma once

#include "physical_device.h"
#include "physical_device_selector.h"

namespace ember::vulkan {

class RankedPhysicalDeviceSelector : public PhysicalDeviceSelector {
public:
	struct RankedCandidate {
		int score;
		PhysicalDevicePtr device;
	};

	using RankedCandidates = std::vector<RankedCandidate>;

	std::optional<PhysicalDevicePtr> select(const PhysicalDevices& candidates) override;

	/// @brief Shortlist the candidates based on their scores. This function evaluates each candidate and returns a list of candidates that meet the minimum score requirement.
	RankedCandidates enumerateEligibleDevices(const PhysicalDevices& candidates);

protected:
	virtual int evaluateCandidate(const PhysicalDevicePtr& candidate, PhysicalDeviceTraits& effectiveTraits) = 0;
};

} // namespace ember::vulkan

