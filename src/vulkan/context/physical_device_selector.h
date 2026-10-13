#pragma once

#include <vulkan/vulkan.h>

#include <optional>
#include <memory>

#include "../vulkan_defs.h"

namespace ember::vulkan {

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
     * Select a physical device from the given candidates.
     * @param candidates A list of candidate device descriptors.
     * @return The selected device, or nulopt if no suitable device can be found.
     */
    virtual std::optional<PhysicalDevicePtr> select(const PhysicalDevices& candidates) = 0;
};

using PhysicalDeviceSelectorPtr = std::unique_ptr<PhysicalDeviceSelector>;

} // namespace ember::vulkan