#pragma once

#include <resource_types.h>

using ShaderID = ember::resources::ShaderID;
using TechniqueID = ember::resources::TechniqueID;
using MaterialID = ember::resources::MaterialID;
using GeometryID = ember::resources::GeometryID;
using VertexFormatID = ember::resources::VertexFormatID;

namespace ember::vulkan {

class VulkanResourceResolver {
public:
	virtual ~VulkanResourceResolver() = default;

    virtual const ShaderRecord& resolveShader(ShaderID id) const = 0;
    virtual const TechniqueRecord& resolveTechnique(TechniqueID id) const= 0;
    virtual const MaterialRecord& resolveMaterial(MaterialID id) const= 0;
    virtual const GeometryRecord& resolveGeometry(GeometryID id) const= 0;
    virtual const VertexFormatRecord& resolveVertexFormat(VertexFormatID id) const= 0;
};

} // namespace ember::vulkan