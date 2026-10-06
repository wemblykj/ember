#pragma once

#include <cstdint>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>

namespace ember::graphics::vulkan {

using ExtensionSet = std::set<std::string>;

struct PhysicalDeviceTraits {
	VkPhysicalDeviceProperties properties{};
	VkPhysicalDeviceFeatures features{};
	ExtensionSet extensions;
	uint32_t queueFamily = 0;
};

struct PhysicalDevice {
	VkPhysicalDevice handle = VK_NULL_HANDLE;
	PhysicalDeviceTraits traits;
};

using PhysicalDevicePtr = std::shared_ptr<PhysicalDevice>;
using PhysicalDevices = std::vector<PhysicalDevicePtr>;

} // namespace ember::graphics::vulkan