#pragma once

#include "resource_types.h"

namespace ember::resources {

/// @brief Built-in resource group IDs for common rendering resource groups.
namespace BuiltinResourceGroup {
    constexpr ResourceGroupID Global = 0;
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

}  // namespace ember::resources