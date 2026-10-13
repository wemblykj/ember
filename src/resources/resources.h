#pragma once

#include <cstdint>
#include <string>

#include "graphics_types.h"

namespace ember::graphics {

using ResourceGroupID = uint32_t;
using TechniqueID = uint32_t;
using MaterialID = uint32_t;
using GeometryID = uint32_t;
using VertexFormatID = uint32_t;

/// @brief Built-in resource group IDs for common rendering resource groups.
namespace BuiltinResourceGroup {
    constexpr ResourceGroupID Default = 0;
    /// @brief Base value for custom resource groups.
    constexpr ResourceGroupID Custom = 0x8000;
}

/// @brief Built-in technique IDs for common rendering techniques.
namespace BuiltinTechnique {
    constexpr TechniqueID Invalid = -1;
    constexpr TechniqueID Undefined = 0;
    constexpr TechniqueID Opaque = 1;
    constexpr TechniqueID Transparent = 2;
    /// @brief Base value for custom techniques.
    constexpr TechniqueID Custom = 0x8000;
}

/// @brief Built-in material IDs for common rendering materials.
namespace BuiltinMaterial {
    constexpr MaterialID Invalid = -1;
    constexpr MaterialID Undefined = 0;
    /// @brief Base value for custom materials.
    constexpr MaterialID Custom = 0x8000;
}

/// @brief Built-in geometry IDs for common rendering geometries.
namespace BuiltinGeometry {
    constexpr GeometryID Invalid = -1;
    constexpr GeometryID Undefined = 0;
    constexpr GeometryID UnitQuad = 1;
    constexpr GeometryID FullscreenQuad = 2;
    /// @brief Base value for custom geometries.
    constexpr GeometryID Custom = 0x8000;
}

namespace BuiltinVertexFormat {
    constexpr VertexFormatID Undefined = 0;
    /// @brief Base value for custom vertex formats.
    constexpr VertexFormatID Custom = 0x8000;
}

struct ResourceGroupDesc {
    std::string debugName;
};

enum class ShaderStage : uint8_t {
    Vertex,
    Fragment,
    Compute,
    Geometry,
    TessellationControl,
    TessellationEvaluation,
    // Mesh, Task — add later if/when needed
};

struct ShaderStageDesc {
    ShaderStage stage;
    std::string entryPoint = "main";
    std::string sourcePath;
};

struct TechniqueDesc {
    std::string name;
    std::vector<ShaderStageDesc> shaderStages;
    VertexFormatID vertexFormat = BuiltinVertexFormat::Undefined;
    BlendMode blendMode = BlendMode::Opaque;
    CullMode cullMode = CullMode::Back;
    bool depthTest = true;
    bool depthWrite = true;
};

struct MaterialDesc {
    std::string name;
    TechniqueID technique = BuiltinTechnique::Opaque;
};

struct GeometryDesc {
    const void* vertexData = nullptr;
    size_t vertexDataSize = 0;
    const void* indexData = nullptr;
    size_t indexDataSize = 0;
    uint32_t indexCount = 0;
    VertexFormatID vertexFormat = BuiltinVertexFormat::Undefined;
};

enum class VertexFormat : uint8_t {
    R32G32_SFLOAT,
    R32G32B32_SFLOAT,
    R32G32B32A32_SFLOAT,
    R8G8B8A8_UNORM,
    A2B10G10R10_SNORM_PACK32,
    // extend as needed
};

struct VertexAttributeDesc {
    uint32_t location;
    VertexFormat format;      // or an engine-level equivalent if you want to stay Vulkan-agnostic
    uint32_t offset;
};

struct VertexFormatDesc {
    uint32_t stride;
    std::vector<VertexAttributeDesc> attributes;
};

}  // namespace ember::graphics