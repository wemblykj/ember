#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <optional>

#include "vulkan_defs.h"

namespace ember::graphics::vulkan {

/**
 * @brief Result of a physical device selection.
 */
struct DeviceSelectionResult {
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties properties{};
    VkPhysicalDeviceFeatures features{};
    ExtensionSet extensions;
    uint32_t queueFamily = 0;
};

/**
 * @brief Selects a physical device from the available devices.
 *
 * Implementations enumerate physical devices, evaluate them according to
 * the provided criteria, and return the best candidate or std::nullopt.
 */
class PhysicalDeviceSelector {
public:
    virtual ~PhysicalDeviceSelector() = default;

    /**
     * Select a physical device for the given instance.
     * @param instance Vulkan instance (already created by context).
     * @return std::optional containing the selection result on success, or std::nullopt on failure.
     */
    virtual std::optional<DeviceSelectionResult> select(VkInstance instance) = 0;
};

using PhysicalDeviceSelectorPtr = std::unique_ptr<PhysicalDeviceSelector>;

} // namespace ember::graphics::vulkan