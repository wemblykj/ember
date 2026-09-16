#pragma once

#include <algorithm>
#include <vector>
#include <cstdint>
#include <ember_math.h>

#include "resources.h"

namespace ember::graphics {

using SortKey = uint64_t;

/// @brief A single render packet, representing a draw call with associated transform, material, and geometry.
struct RenderPacket {
    core::Mat4 transform;
    MaterialID materialId;
    GeometryID geometryId;
    SortKey sortKey;
};

using RenderQueue = std::vector<RenderPacket>;

/// @brief Sorting policy for render packets within a pass.
enum class SortPolicy : uint8_t {
    MaterialMajor,
    DepthMajor
};

/// @brief Sorts the render packets in the given queue according to the specified sorting policy.
inline void sortPass(RenderQueue& queue, SortPolicy sortPolicy) {
    if (sortPolicy == SortPolicy::MaterialMajor) {
        std::sort(queue.begin(), queue.end(),
            [](const RenderPacket& a, const RenderPacket& b) {
                if (a.materialId != b.materialId) return a.materialId < b.materialId;
                return a.sortKey < b.sortKey;
            });
    }
    else { // DepthMajor
        std::sort(queue.begin(), queue.end(),
            [](const RenderPacket& a, const RenderPacket& b) {
                return a.sortKey > b.sortKey; // back-to-front
            });
    }
}

}  // namespace ember::graphics