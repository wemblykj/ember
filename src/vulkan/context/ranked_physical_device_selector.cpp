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
		RankedCandidate rankedCandidate{
			.device = std::make_shared<PhysicalDevice>()
		};

		// evaluate the candidate and assign a score
		rankedCandidate.score = evaluateCandidate(candidate, rankedCandidate.device->traits);

		if (rankedCandidate.score >= 0) {
			rankedCandidate.device->handle = candidate->handle;
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