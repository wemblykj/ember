#pragma once

#include <cstdint>

namespace ember::resources {

using ResourceGroupID = uint32_t;
using TechniqueID = uint32_t;
using MaterialID = uint32_t;
using GeometryID = uint32_t;
using VertexFormatID = uint32_t;

enum class VertexFormat : uint8_t {
    R32G32_SFLOAT,
    R32G32B32_SFLOAT,
    R32G32B32A32_SFLOAT,
    R8G8B8A8_UNORM,
    A2B10G10R10_SNORM_PACK32,
    // extend as needed
};

}  // namespace ember::resources