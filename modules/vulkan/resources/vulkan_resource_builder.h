#pragma once

#include <resource_types.h>

#include "vulkan_resource_defs.h"
#include "vulkan_resource_resolver.h"

using ShaderID = ember::resources::ShaderID;
using TechniqueID = ember::resources::TechniqueID;
using MaterialID = ember::resources::MaterialID;
using GeometryID = ember::resources::GeometryID;
using VertexFormatID = ember::resources::VertexFormatID;

using ShaderDesc = ember::resources::ShaderDesc;
using TechniqueDesc = ember::resources::TechniqueDesc;
using MaterialDesc = ember::resources::MaterialDesc;
using GeometryDesc = ember::resources::GeometryDesc;
using VertexFormatDesc = ember::resources::VertexFormatDesc;

namespace ember::vulkan {

/**
 * @brief Builds backend-agnostic resource descriptions into Vulkan GPU objects.
 *
 * Implementations handle shader compilation (e.g. via shaderc), pipeline
 * creation, descriptor layout generation, and buffer/image uploads. The
 * resource cache calls this interface during registration; it does not
 * perform compilation itself.
 */
class VulkanResourceBuilder {
public:
    virtual ~VulkanResourceBuilder() = default;

	virtual bool buildCannedShader(ShaderID id, ShaderRecord& record) = 0;
    virtual bool buildCannedTechnique(TechniqueID id, const VulkanResourceResolver& resourceResolver, TechniqueRecord& record) = 0;
    virtual bool buildCannedMaterial(MaterialID id, MaterialRecord& record) = 0;
    virtual bool buildCannedGeometry(GeometryID id, GeometryRecord  & record) = 0;
    virtual bool buildCannedVertexFormat(VertexFormatID id, VertexFormatRecord& record) = 0;

	virtual bool buildShader(const ShaderDesc& desc, ShaderRecord& record) = 0;
    virtual bool buildTechnique(const TechniqueDesc& desc, const VulkanResourceResolver& resourceResolver, TechniqueRecord& record) = 0;
    virtual bool buildMaterial(const MaterialDesc& desc, MaterialRecord& record) = 0;
    virtual bool buildGeometry(const GeometryDesc& desc, GeometryRecord& record, GeometryAllocation& allocation) = 0;
    virtual bool buildVertexFormat(const VertexFormatDesc& desc, VertexFormatRecord& record) = 0;
    
    virtual void destroyShader(ShaderRecord& record) = 0;
    virtual void destroyTechnique(TechniqueRecord& record) = 0;
    virtual void destroyMaterial(MaterialRecord& record) = 0;
    virtual void destroyGeometry(GeometryRecord& record, GeometryAllocation& allocation) = 0;
    virtual void destroyVertexFormat(VertexFormatRecord& record) = 0;
};

} // namespace ember::vulkan