#pragma once

#include <cstdint>
#include <string>

#include "graphics_types.h"

namespace ember::graphics {

using ResourceGroupID = uint32_t;
using TechniqueID = uint32_t;
using MaterialID = uint32_t;
using GeometryID = uint32_t;

/// @brief Built-in resource group IDs for common rendering resource groups.
namespace BuiltinResourceGroup {
    constexpr ResourceGroupID Default = 0;
    /// @brief Base value for custom resource groups.
    constexpr ResourceGroupID Custom = 0x8000;
}

/// @brief Built-in technique IDs for common rendering techniques.
namespace BuiltinTechnique {
    constexpr TechniqueID Opaque = 0;
    constexpr TechniqueID Transparent = 1;
    /// @brief Base value for custom techniques.
    constexpr TechniqueID Custom = 0x8000;
}

/// @brief Built-in material IDs for common rendering materials.
namespace BuiltinMaterial {
    constexpr MaterialID Undefined = 0;
    /// @brief Base value for custom materials.
    constexpr MaterialID Custom = 0x8000;
}

/// @brief Built-in geometry IDs for common rendering geometries.
namespace BuiltinGeometry {
    constexpr GeometryID FullscreenQuad = 1;
    /// @brief Base value for custom geometries.
    constexpr GeometryID Custom = 0x8000;
}

struct ShaderStageDesc {
    std::string entryPoint = "main";
    std::string sourcePath;
};

struct ResourceGroupDesc {
    std::string debugName;
};

struct TechniqueDesc {
    std::string name;
    ShaderStageDesc vertexShader;
    ShaderStageDesc fragmentShader;
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
};

}  // namespace ember::graphics