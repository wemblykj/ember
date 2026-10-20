#pragma once

#include <vector>

#include "physical_device.h"

namespace ember::vulkan {

/**
 * @brief Enumerates the physical devices available from the given Vulkan instance.
 *
 * Implementations enumerate physical devices, evaluate them according to the provided criteria, and return a list of suitable candidates.
*/
class PhysicalDeviceEnumerator {
public:
    virtual ~PhysicalDeviceEnumerator() = default;

    /**
        * Enumerate the physical devices available from the given Vulkan instance.
        * @return A list of available candidate devices.
        */
    virtual std::vector<PhysicalDevicePtr> enumerate() = 0;
};

} // namespace ember::vulkan