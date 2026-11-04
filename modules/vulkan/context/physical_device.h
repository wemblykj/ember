#pragma once

#include <vector>
#include <memory>

#include <vulkan/vulkan.h>

#include <cstdint>

#include "vulkan_defs.h"

struct QueueTraits {
	float priority = 1.0;
};

struct QueueFamilyTraits {
	
	uint32_t queueFamilyIndex = 0;
	VkQueueFlags flags = 0;
	std::vector<QueueTraits> queues;
};

struct PhysicalDeviceTraits {
	VkPhysicalDeviceProperties properties{};
	VkPhysicalDeviceFeatures features{};
	ExtensionSet extensions;
	std::vector<QueueFamilyTraits> queueFamilies;
};

struct PhysicalDevice {
	VkPhysicalDevice handle = VK_NULL_HANDLE;
	PhysicalDeviceTraits traits;
};

using PhysicalDevicePtr = std::shared_ptr<PhysicalDevice>;
using PhysicalDevices = std::vector<PhysicalDevicePtr>;
