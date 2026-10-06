#include "default_physical_device_selector.h"

#include <algorithm>

namespace ember::graphics::vulkan {
	DefaultPhysicalDeviceSelector& DefaultPhysicalDeviceSelector::allowDeviceTypes(std::initializer_list<VkPhysicalDeviceType> types) {
		allowedDeviceTypes_.insert(types.begin(), types.end());

		return *this;
	}

	DefaultPhysicalDeviceSelector& DefaultPhysicalDeviceSelector::preferDeviceType(VkPhysicalDeviceType type, int weight)
	{
		preferredDeviceTypes_[type] = weight;

		return *this;
	}

	DefaultPhysicalDeviceSelector& DefaultPhysicalDeviceSelector::requireExtensions(ExtensionSet extensions) {
	for (const auto& ext : extensions) {
		requiredExtensions_.insert(ext);
	}
	return *this;
}

DefaultPhysicalDeviceSelector& DefaultPhysicalDeviceSelector::preferExtensions(ExtensionSet extensions, int weight) {
	for (const auto& ext : extensions) {
		preferredExtensions_.insert({ext, weight});
	}
	return *this;
}

DefaultPhysicalDeviceSelector& DefaultPhysicalDeviceSelector::requireSurfaceSupport(VkSurfaceKHR surface)
{ 
	requiredSurfaceSupport_ = surface; 
	return *this; 
}

int DefaultPhysicalDeviceSelector::evaluateCandidate(const PhysicalDevicePtr& candidate, PhysicalDeviceTraits& effectiveTraits)
{
	if (requiredDeviceType_ && candidate->traits.properties.deviceType != *requiredDeviceType_) {
		return -1; // Candidate does not match the required device type
	}

	if (!allowedDeviceTypes_.empty() && allowedDeviceTypes_.find(candidate->traits.properties.deviceType) == allowedDeviceTypes_.end()) {
		return -1; // Candidate is not in the allowed device types
	}

	if (!disallowedDeviceTypes_.empty() && disallowedDeviceTypes_.find(candidate->traits.properties.deviceType) != disallowedDeviceTypes_.end()) {
		return -1; // Candidate is in the disallowed device types
	}

	if (!requiredFeatures_.empty()) {
		for (const auto& feature : requiredFeatures_) {
			if (memcmp(&candidate->traits.features, &feature, sizeof(VkPhysicalDeviceFeatures)) != 0) {
				return -1; // Candidate does not support the required features
			}
		}
	}

	if (!requiredQueueFlags_.empty() && (candidate->traits.queueFamily & *requiredQueueFlags_.begin()) != *requiredQueueFlags_.begin()) {
		return -1; // Candidate does not support the required queue flags
	}

	if (!requiredExtensions_.empty()) {
		for (const auto& ext : requiredExtensions_) {
			if (candidate->traits.extensions.find(ext) == candidate->traits.extensions.end()) {
				return -1; // Candidate does not support the required extensions
			}
		}
	}
	if (requiredSurfaceSupport_ != VK_NULL_HANDLE) {
		VkBool32 presentSupport = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(candidate->handle, candidate->traits.queueFamily, requiredSurfaceSupport_, &presentSupport);
		if (presentSupport == VK_FALSE) {
			return -1; // Candidate does not support presentation on the given surface
		}
	}

	int totalScore = 0;
	int totalWeight = 0;

	const auto preferredDeviceTypeIt = preferredDeviceTypes_.find(candidate->traits.properties.deviceType);
	if (preferredDeviceTypeIt != preferredDeviceTypes_.end()) {
		const auto& weight = preferredDeviceTypeIt->second;

		totalScore += weight;
		totalWeight += weight;
	}
	
	const auto preferredVendorIt = preferredVendors_.find(candidate->traits.properties.vendorID);
	if (preferredVendorIt != preferredVendors_.end()) {
		const auto& weight = preferredVendorIt->second;

		totalScore += weight;
		totalWeight += weight;
	}

	constexpr size_t NumFeatures = sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32);
	for (const auto& entry : preferredFeaturesSets_) {
		const auto& featureSet = entry.first;
		const auto& setWeight = entry.second;
		
		const VkBool32* deviceFeatures = reinterpret_cast<const VkBool32*>(&candidate->traits.features);
		const VkBool32* preferredFeatures = reinterpret_cast<const VkBool32*>(&featureSet);
		VkBool32* enabledFeatures = reinterpret_cast<VkBool32*>(&effectiveTraits.features);
		for (int i = 0; i < NumFeatures; ++i) {
			if (preferredFeatures[i] == VK_TRUE && deviceFeatures[i] == VK_TRUE) {
				totalScore += setWeight;
				totalWeight += setWeight;

				enabledFeatures[i] = VK_TRUE;
			}
		}
	}
	
	for (const auto& deviceExtension : candidate->traits.extensions) {
		const auto preferredExtensionIt = preferredExtensions_.find(deviceExtension);
		if (preferredExtensionIt != preferredExtensions_.end()) {
			const auto& extension = preferredExtensionIt->first;
			const auto& weight = preferredExtensionIt->second;

			totalScore += weight;
			totalWeight += weight;

			effectiveTraits.extensions.insert(extension);
		}
	}
	
	for (const auto& entry : preferredQueueFlagsSets_) {
		const auto& preferredFeatureSet = entry.first;
		const auto& setWeight = entry.second;

		uint32_t overlappedFeatures = candidate->traits.queueFamily & preferredFeatureSet;
		effectiveTraits.queueFamily |= overlappedFeatures;
		do {
			if (overlappedFeatures & 1) {
				totalScore += setWeight;
				totalWeight += setWeight;
			}
		} while (overlappedFeatures >> 1);
	}

	for (const auto& entry : customScorers_) {
		const CustomScorerFn& customScorer = entry.first;
		const auto& weight = entry.second;

		totalScore += customScorer(candidate, effectiveTraits) * weight;
		totalWeight += weight;
	}

	const auto score = totalScore / totalWeight;
	
	return score;
}

DefaultPhysicalDeviceSelectorPtr createDefaultPhysicalDeviceSelector()
{
	return std::make_unique<DefaultPhysicalDeviceSelector>();
}

}
