#pragma once

#include <cstdint>
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

struct GeometryDesc {
    const void* vertexData = nullptr;
    size_t vertexDataSize = 0;
    const void* indexData = nullptr;
    size_t indexDataSize = 0;
    uint32_t indexCount = 0;
    VertexFormatID vertexFormat = BuiltinVertexFormat::Undefined;
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

}  // namespace ember::resources