#pragma once

#include <cstdint>

namespace ember::resources {

using ResourceGroupID = uint32_t;
using ShaderID = uint32_t;
using TechniqueID = uint32_t;
using MaterialID = uint32_t;
using GeometryID = uint32_t;
using VertexFormatID = uint32_t;

enum class ShaderType : uint8_t {
    Vertex,
    Fragment,
    Compute,
    Geometry,
    TessellationControl,
    TessellationEvaluation,
    // Mesh, Task — add later if/when needed
};

/// @brief Index buffer formats for use in geometry resources.
enum class IndexType : uint8_t {
    Undefined,
    UInt16,
    UInt32,
};

/// @brief Vertex attribute formats for use in vertex buffers and input layouts.
enum class VertexFormat : uint8_t {
    Undefined,
    R32G32_SFLOAT,
    R32G32B32_SFLOAT,
    R32G32B32A32_SFLOAT,
    R8G8B8A8_UNORM,
    A2B10G10R10_SNORM_PACK32,
    // extend as needed
};

}  // namespace ember::resources