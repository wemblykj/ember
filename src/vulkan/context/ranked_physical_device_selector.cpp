#include "ranked_physical_device_selector.h"

#include <algorithm>

namespace ember::graphics::vulkan {

std::optional<DeviceSelectionResult> RankedPhysicalDeviceSelector::select(VkInstance instance)
{
	auto candidates = this->candidates(instance);
	if (candidates.empty()) {
		return std::nullopt;
	}
	
	// Return the best candidate
	return candidates.front();
}

std::vector<DeviceSelectionResult> RankedPhysicalDeviceSelector::candidates(
	VkInstance instance)
{
	auto ranked = rankedCandidates(instance);
	std::vector<DeviceSelectionResult> results;
	for (const auto& [score, result] : ranked) {
		results.push_back(result);
	}

	return results;
}

std::vector<RankedPhysicalDeviceSelector::RankedCandidate> RankedPhysicalDeviceSelector::rankedCandidates(VkInstance instance)
{
	std::vector<RankedCandidate> candidates;

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0) {
		return candidates; // No devices found
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
	for (const auto& device : devices) {
		DeviceSelectionResult result{ .physicalDevice = device };

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

		// Evaluate the device and assign a score
		auto score = evaluateCandidate(result);

		candidates.emplace_back(score, result);
	}

	// Sort candidates by score in descending order
	std::sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
		auto [scoreA, resultA] = a;
		auto [scoreB, resultB] = b;
		return scoreA > scoreB;
		});

	return candidates;
}

} // namespace ember::graphics::vulkan