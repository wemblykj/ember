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
    std::string name;
};

struct ShaderStages {
    ShaderID vertex = BuiltinShader::GenericTransform;      // Required
    ShaderID fragment = BuiltinShader::OpaquePBR;           // Required
    ShaderID geometry = BuiltinShader::Undefined;           // Optional
    ShaderID tessControl = BuiltinShader::Undefined;        // Optional
    ShaderID tessEval = BuiltinShader::Undefined;           // Optional
    ShaderID mesh = BuiltinShader::Undefined;               // Optional
	ShaderID compute = BuiltinShader::Undefined;            // Optional
};

struct ShaderDesc {
    std::string name;
    ShaderType type;
    std::string entryPoint = "main";
    std::string sourcePath;
};


struct TechniqueDesc {
    std::string name;
    core::BlendMode blendMode = core::BlendMode::Opaque;
    core::CullMode cullMode = core::CullMode::Back;
    bool depthTest = true;
    bool depthWrite = true;
	ShaderStages shaderStages;
    VertexFormatID vertexFormat = BuiltinVertexFormat::Undefined;
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