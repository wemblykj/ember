#pragma once

#include <cstdint>
#include <span>
#include <string>
#include <vector>

#include <graphics_types.h>

#include "resource_types.h"
#include "canned_resources.h"

namespace ember::resources {

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
    core::BlendMode blendMode = core::BlendMode::Opaque;
    core::CullMode cullMode = core::CullMode::Back;
    bool depthTest = true;
    bool depthWrite = true;
};

struct MaterialDesc {
    std::string name;
    TechniqueID technique = BuiltinTechnique::Opaque;
};

/**
 * @brief Backend-agnostic description of geometry data (vertex/index buffers).
 *
 * Vertex data is always required. Index data is optional — if indexType is
 * Undefined (or indexData is empty), this geometry has no index buffer and
 * must be drawn directly from vertex data (vkCmdDraw) using vertexCount.
 * Indexed geometry uses indexCount/indexType instead.
 */
struct GeometryDesc {
    std::string name;

    // --- Vertex data (always required) ---
    std::span<const std::byte> vertexData;
    uint32_t vertexCount = 0;
    VertexFormatID vertexFormat = BuiltinVertexFormat::Undefined;

    // --- Index data (optional — absent for non-indexed geometry) ---
    std::span<const std::byte> indexData;
    uint32_t indexCount = 0;
    IndexType indexType = IndexType::Undefined;
};

struct VertexAttributeDesc {
    std::string name;
    uint32_t location;
    VertexFormat format;      // or an engine-level equivalent if you want to stay Vulkan-agnostic
    uint32_t offset;
};

struct VertexFormatDesc {
    std::string name;
    uint32_t stride;
    std::vector<VertexAttributeDesc> attributes;
};

}  // namespace ember::resources