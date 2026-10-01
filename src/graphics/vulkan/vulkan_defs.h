#pragma once

#include <set>

using ExtensionSet = std::set<std::string>;

/**
 * @brief Structure representing an allocation handle for Vulkan resources. This structure contains information about the backend used for memory allocation and an opaque handle to the allocated memory.
 */
struct AllocationHandle {
    enum class Backend : uint8_t { None = 0, Vma, Vk } backend = Backend::None; uintptr_t handle = 0; // opaque storage for backend-specific handle
};