#include "ranked_physical_device_selector.h"

#include <algorithm>

namespace ember::vulkan {

std::optional<PhysicalDevicePtr> RankedPhysicalDeviceSelector::select(const PhysicalDevices& candidates)
{
	auto selectionCandidates = this->enumerateEligibleDevices(candidates);
	if (selectionCandidates.empty()) {
		return std::nullopt;
	}

	// Return the best candidate
	return selectionCandidates.front().device;
}

RankedPhysicalDeviceSelector::RankedCandidates RankedPhysicalDeviceSelector::enumerateEligibleDevices(const PhysicalDevices& candidates)
{
	RankedCandidates selectionCandidates;

	for (const auto& candidate : candidates) {
		// evaluate the candidate and assign a score
		PhysicalDeviceTraits effectiveTraits; // Copy the traits to modify them if necessary
		auto score = evaluateCandidate(candidate, effectiveTraits);

		if (score >= 0) {
			auto device = std::make_shared<PhysicalDevice>();
			device->handle = candidate->handle;
			device->traits = effectiveTraits;

			RankedCandidate rankedCandidate{
			.score = score,
			.device = device
			};

			selectionCandidates.emplace_back(rankedCandidate);
		}
	}

	// Sort candidates by score in descending order
	std::sort(selectionCandidates.begin(), selectionCandidates.end(), [](const auto& a, const auto& b) {
		return a.score > b.score;
		});

	return selectionCandidates;
}

} // namespace ember::vulkan