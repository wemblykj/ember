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

/*
RankedPhysicalDeviceSelector::PhysicalDeviceCandidate RankedPhysicalDeviceSelector::toCandidate(PhysicalDeviceInfo deviceInfo) {
	
	const auto& device = deviceInfo.physicalDevice;

	PhysicalDeviceCandidate result{ .physicalDevice = device };

	vkGetPhysicalDeviceProperties(device, &result.properties);

	vkGetPhysicalDeviceFeatures(device, &result.features);

	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	if (extensionCount > 0) {
		std::vector<VkExtensionProperties> available(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, available.data());

		std::ranges::transform(available, std::inserter(result.extensions, result.extensions.end()),
			[](const VkExtensionProperties& ext) { return ext.extensionName; });
	}
	
	return result;
}*/

} // namespace ember::vulkan