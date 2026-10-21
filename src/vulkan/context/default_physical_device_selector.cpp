#include "default_physical_device_selector.h"

#include <algorithm>
#include <bit>
#include <ranges>

namespace ember::vulkan {

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
		preferredExtensions_.insert({ ext, weight });
	}
	return *this;
}

DefaultPhysicalDeviceSelector& DefaultPhysicalDeviceSelector::requireFeatures(const VkPhysicalDeviceFeatures& features)
{
	const VkBool32* requireFeatures = reinterpret_cast<const VkBool32*>(&features);
	constexpr auto MaxFeatureCount = sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32);
	for (int i = 0; i < MaxFeatureCount; ++i) {
		if (requireFeatures[i] == VK_TRUE) {
			requiredFeatureIndices_.insert(i);
		}
	}

	return *this;
}

DefaultPhysicalDeviceSelector& DefaultPhysicalDeviceSelector::preferFeatures(const VkPhysicalDeviceFeatures& features, int weight)
{
	const VkBool32* requireFeatures = reinterpret_cast<const VkBool32*>(&features);
	constexpr auto MaxFeatureCount = sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32);
	for (int i = 0; i < MaxFeatureCount; ++i) {
		if (requireFeatures[i] == VK_TRUE) {
			preferredFeatureIndexWeights_.insert({ i, weight });
		}
	}

	return *this;
}

DefaultPhysicalDeviceSelector& DefaultPhysicalDeviceSelector::requireQueueFlags(VkQueueFlags flags) {
	requiredQueueFlags_ |= flags;

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

	if (!requiredFeatureIndices_.empty()) {
		const VkBool32* deviceFeatures = reinterpret_cast<const VkBool32*>(&candidate->traits.features);
		VkBool32* enabledFeatures = reinterpret_cast<VkBool32*>(&effectiveTraits.features);

		for (const auto& featureIndex : requiredFeatureIndices_) {
			if (deviceFeatures[featureIndex] == VK_FALSE) {
				return -1; // Candidate does not support the required feature
			}

			enabledFeatures[featureIndex] = VK_TRUE; // Enable the required feature in the effective traits
		}
	}

	if (!requiredExtensions_.empty()) {
		for (const auto& extension : requiredExtensions_) {
			if (candidate->traits.extensions.find(extension) == candidate->traits.extensions.end()) {
				return -1; // Candidate does not support the required extensions
			}

			effectiveTraits.extensions.insert(extension);
		}
	}

	std::list<QueueFamilyTraits> candidateQueueFamilies(candidate->traits.queueFamilies.begin(), candidate->traits.queueFamilies.end());

	if (requiredQueueFlags_ != 0) {
		const auto& requiredQueueFlags = requiredQueueFlags_;
		for (auto queueFamily : candidate->traits.queueFamilies) {
			if (queueFamily.queues.size() == 0 ||
				(queueFamily.flags & requiredQueueFlags_) == 0) {
					std::erase_if(candidateQueueFamilies, [&](const QueueFamilyTraits& qf) {
						return qf.queueFamilyIndex == queueFamily.queueFamilyIndex;
					}); // Candidate does not support the required queue flags
				}
		}

		if (candidateQueueFamilies.empty()) {
			return -1; // Candidate does not support the required queue flags
		}
	}

	if (requiredSurfaceSupport_ != VK_NULL_HANDLE) {
		auto it = candidateQueueFamilies.begin();
		while (it != candidateQueueFamilies.end()) {
			if (it->queues.empty())
				continue; // Candidate does not have any queues in this family
			VkBool32 presentSupport = VK_FALSE;
			vkGetPhysicalDeviceSurfaceSupportKHR(candidate->handle, it->queueFamilyIndex, requiredSurfaceSupport_, &presentSupport);
			if (presentSupport == VK_FALSE) {
				it = candidateQueueFamilies.erase(it);
			} else {
				++it;
			}
		}

		if (candidateQueueFamilies.empty()) {
			return -1; // No candidate supports presentation on the given surface
		}
	}

	int deviceScore = 0;

	const auto preferredDeviceTypeIt = preferredDeviceTypes_.find(candidate->traits.properties.deviceType);
	if (preferredDeviceTypeIt != preferredDeviceTypes_.end()) {
		const auto& weight = preferredDeviceTypeIt->second;

		deviceScore += weight;
	}
	
	const auto preferredVendorIt = preferredVendors_.find(candidate->traits.properties.vendorID);
	if (preferredVendorIt != preferredVendors_.end()) {
		const auto& weight = preferredVendorIt->second;

		deviceScore += weight;
	}

	if (!preferredFeatureIndexWeights_.empty()) {
		const int preferredFeaturesCount = preferredFeatureIndexWeights_.size();

		const VkBool32* deviceFeatures = reinterpret_cast<const VkBool32*>(&candidate->traits.features);
		VkBool32* enabledFeatures = reinterpret_cast<VkBool32*>(&effectiveTraits.features);
		
		int totalFeaturesScore = 0;

		for (const auto& entry : preferredFeatureIndexWeights_) {
			const auto& featureIndex = entry.first;
			const auto& featureWeight = entry.second;

			if (deviceFeatures[featureIndex] == VK_TRUE) {
				enabledFeatures[featureIndex] = VK_TRUE;
				totalFeaturesScore += featureWeight;
			}
		}

		deviceScore += totalFeaturesScore / preferredFeaturesCount;
	}
	
	if (!preferredExtensions_.empty()) {
		const int preferredExtensionsCount = preferredExtensions_.size();
		int totalExtensionsScore = 0;
		for (const auto& entry : preferredExtensions_) {
			const auto& extension = entry.first;
			const auto& weight = entry.second;
			if (candidate->traits.extensions.find(extension) != candidate->traits.extensions.end()) {
				totalExtensionsScore += weight;
				effectiveTraits.extensions.insert(extension);
			}
		}
		deviceScore += totalExtensionsScore / preferredExtensionsCount;
	}
	
	/*
	if (!preferredQueueFlagsSets_.empty()) {
		std::list<std::pair<int, QueueFamilyTraits>> preferredQueueFamilies;
		const int preferredQueueFamilyCount = preferredQueueFlagsSets_.size();
		int totalQueueFamilyScore = 0;
		for (const auto& entry : preferredQueueFlagsSets_) {
			const auto& preferredQueueFlags = entry.first;
			const auto& weight = entry.second;
			int queueFamilyScore = 0;
			for (const auto queueFamily : candidateQueueFamilies) {
				if (queueFamily.queueCount == 0)
					continue; // Candidate does not have any queues in this family

				if ((queueFamily.flags & preferredQueueFlags) == preferredQueueFlags) {
					
					queueFamilyScore += weight;
					preferredQueueFamilies.emplace_back(weight, queueFamily);
					break; // Found a matching queue family, no need to check further
				}
			}
		}

		std::ranges::sort(preferredQueueFamilies, std::greater{}, [](const auto& f) { return f.first; });


		totalScore += totalQueueFamilyScore / preferredQueueFamilyCount;
	else {			// If no preferred queue families add all candidate queue families to the effective traits
		for (const auto queueFamily : candidateQueueFamilies) {
			effectiveTraits.queueFamilies.push_back(queueFamily);
		}
	}*/

	for (const auto& entry : customScorers_) {
		const CustomScorerFn& customScorer = entry.first;
		const auto& weight = entry.second;

		deviceScore += customScorer(candidate, effectiveTraits) * weight;
	}

	// all requirements are met, copy the candidate's core traits to the effective traits
	effectiveTraits.	properties = candidate->traits.properties;
	std::ranges::copy(candidateQueueFamilies, std::back_inserter(effectiveTraits.queueFamilies));

	return deviceScore;
}

DefaultPhysicalDeviceSelectorPtr createDefaultPhysicalDeviceSelector()
{
	return std::make_unique<DefaultPhysicalDeviceSelector>();
}

}
