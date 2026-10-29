#pragma once

#include "resource_types.h"

namespace ember::resources {

/// @brief Built-in resource group IDs for common rendering resource groups.
namespace CannedResourceGroup {
    constexpr ResourceGroupID Global = 0;
    /// @brief Base value for custom resource groups.
    constexpr ResourceGroupID Custom = 0x8000;
}

/// @brief Built-in shader IDs for common rendering shaders.
namespace CannedShader {
	constexpr ShaderID Invalid = 0x80000000;
	constexpr ShaderID Undefined = Invalid + 1;
    constexpr ShaderID Vertex = 0x1000;
    constexpr ShaderID InvalidVertex = Invalid | Vertex;
    constexpr ShaderID UndefinedVertex = Undefined | Vertex;
    constexpr ShaderID GenericTransform = Vertex + 1;
    constexpr ShaderID Fragment = 0x2000;
    constexpr ShaderID InvalidFragment = Invalid | Fragment;
    constexpr ShaderID UndefinedFragment = Undefined | Fragment;
	constexpr ShaderID OpaquePBR = Fragment + 1;
	/// @brief Base value for custom shaders.
	constexpr ShaderID Custom = 0x8000;
}

/// @brief Built-in technique IDs for common rendering techniques.
namespace CannedTechnique {
    constexpr TechniqueID Invalid = 0x80000000;
    constexpr TechniqueID Undefined = Invalid + 1;
    constexpr TechniqueID Opaque = 1;
    constexpr TechniqueID Transparent = 2;
    /// @brief Base value for custom techniques.
    constexpr TechniqueID Custom = 0x8000;
}

/// @brief Built-in material IDs for common rendering materials.
namespace CannedMaterial {
    constexpr MaterialID Invalid = 0x80000000;
    constexpr MaterialID Undefined = Invalid + 1;
    /// @brief Base value for custom materials.
    constexpr MaterialID Custom = 0x8000;
}

/// @brief Built-in geometry IDs for common rendering geometries.
namespace CannedGeometry {
    constexpr GeometryID Invalid = 0x80000000;
    constexpr GeometryID Undefined = Invalid + 1;
    constexpr GeometryID UnitQuad = 1;
    constexpr GeometryID FullscreenQuad = 2;
    /// @brief Base value for custom geometries.
    constexpr GeometryID Custom = 0x8000;
}

namespace CannedVertexFormat {
    constexpr VertexFormatID Invalid = 0x8000;
    constexpr VertexFormatID Undefined = Invalid + 1;
    constexpr VertexFormatID PositionUV = 1;
    /// @brief Base value for custom vertex formats.
    constexpr VertexFormatID Custom = 0x8000;
}

}  // namespace ember::resources