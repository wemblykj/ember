#pragma once

#include <vulkan/vulkan.h>

#include <map>
#include <set>
#include <functional>
#include <memory>
#include <optional>
#include <string>

#include "ranked_physical_device_selector.h"

namespace ember::graphics::vulkan {

class DefaultPhysicalDeviceSelector : public RankedPhysicalDeviceSelector {
public:
		
	using CustomScorerFn = std::function<int(const PhysicalDevicePtr&, PhysicalDeviceTraits&)>;

	/// @brief Set the minimum score required for a physical device to be considered.
	DefaultPhysicalDeviceSelector& setMinScore(int minScore);

	/// @brief Require a specific type of physical device.
	DefaultPhysicalDeviceSelector& requireDeviceType(VkPhysicalDeviceType type);

	/// @brief Allow only specific types of physical device.
	DefaultPhysicalDeviceSelector& allowDeviceTypes(std::initializer_list<VkPhysicalDeviceType> types);

	/// @brief Disallow specific types of physical device.
	DefaultPhysicalDeviceSelector& disallowDeviceTypes(std::initializer_list<VkPhysicalDeviceType> types);

	/// @brief Prefer a specific type of physical device, with an optional weight.
	DefaultPhysicalDeviceSelector& preferDeviceType(VkPhysicalDeviceType type, int weight = 100);
	
	/// @brief Require specific extensions.
	DefaultPhysicalDeviceSelector& requireExtensions(ExtensionSet extensions);

	/// @brief Prefer specific extensions, with an optional weight.
	DefaultPhysicalDeviceSelector& preferExtensions(ExtensionSet, int weight = 10);
	
	/// @brief Require specific features.
	DefaultPhysicalDeviceSelector& requireFeatures(const VkPhysicalDeviceFeatures& features);
	/// @brief Prefer specific features, with an optional weight.
	DefaultPhysicalDeviceSelector& preferFeatures(const VkPhysicalDeviceFeatures& features, int weight = 10);

	/// @brief Require specific queue flags.
	DefaultPhysicalDeviceSelector& requireQueueFlags(VkQueueFlags flags);
	/// @brief Prefer specific queue flags, with an optional weight.
	DefaultPhysicalDeviceSelector& preferQueueFamily(uint32_t familyIndex, int weight = 50);

	/// @brief Require a specific vendor ID.
	DefaultPhysicalDeviceSelector& preferVendor(uint32_t vendorId, int weight = 50);

	/// @brief Require that the physical device supports a specific surface.
	DefaultPhysicalDeviceSelector& requireSurfaceSupport(VkSurfaceKHR surface);

	/// @brief Set a custom scorer function to evaluate device selection results.
	DefaultPhysicalDeviceSelector& addCustomScorer(CustomScorerFn scorer, int weight = 50);

private:
	int evaluateCandidate(const PhysicalDevicePtr& candidate, PhysicalDeviceTraits& effectiveTraits) override;

private:
	std::optional<VkPhysicalDeviceType>	requiredDeviceType_;
	std::set<VkPhysicalDeviceType> allowedDeviceTypes_;
	std::set<VkPhysicalDeviceType> disallowedDeviceTypes_;
	std::map<VkPhysicalDeviceType, int> preferredDeviceTypes_;
	std::set<VkPhysicalDeviceFeatures> requiredFeatures_;
	std::vector<std::pair<VkPhysicalDeviceFeatures, int>> preferredFeaturesSets_;
	std::set<VkQueueFlags> requiredQueueFlags_;
	std::map<VkQueueFlags, int> preferredQueueFlagsSets_;
	ExtensionSet requiredExtensions_;
	std::map<std::string, int> preferredExtensions_;
	std::map<uint32_t, int> preferredVendors_;
	std::map<CustomScorerFn, int> customScorers_;
	int customScorerWeight_ = 50;

	VkSurfaceKHR requiredSurfaceSupport_ = VK_NULL_HANDLE;

};

using DefaultPhysicalDeviceSelectorPtr = std::unique_ptr<DefaultPhysicalDeviceSelector>;

DefaultPhysicalDeviceSelectorPtr createDefaultPhysicalDeviceSelector();

} // namespace ember::graphics::vulkan

