#pragma once

#include <optional>
#include <memory>
#include <vector>

#include "vulkan_defs.h" 
#include "vulkan_instance.h" 

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
    virtual std::vector<PhysicalDevicePtr> enumerate(const VulkanInstancePtr& instance) = 0;
};

} // namespace ember::vulkan