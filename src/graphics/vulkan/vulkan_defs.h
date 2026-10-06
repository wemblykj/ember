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

/**
 * @brief Structure representing an allocation handle for Vulkan resources. This structure contains information about the backend used for memory allocation and an opaque handle to the allocated memory.
 */
struct AllocationHandle {
	enum class Backend : uint8_t { None = 0, Vma, Vk } backend = Backend::None; uintptr_t handle = 0; // opaque storage for backend-specific handle
};

} // namespace ember::graphics::vulkan
