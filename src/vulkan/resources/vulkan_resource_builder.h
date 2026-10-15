#pragma once

#include <resource_types.h>

#include "vulkan_resource_defs.h"

using TechniqueID = ember::resources::TechniqueID;
using MaterialID = ember::resources::MaterialID;
using GeometryID = ember::resources::GeometryID;

using TechniqueDesc = ember::resources::TechniqueDesc;
using MaterialDesc = ember::resources::MaterialDesc;
using GeometryDesc = ember::resources::GeometryDesc;

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

    virtual bool buildCannedTechnique(TechniqueID id, TechniqueRecord& record) = 0;
    virtual bool buildCannedMaterial(MaterialID id, MaterialRecord& record) = 0;
    virtual bool buildCannedGeometry(GeometryID id, GeometryRecord  & record) = 0;

    virtual bool buildTechnique(const TechniqueDesc& desc, TechniqueRecord& record) = 0;
    virtual bool buildMaterial(const MaterialDesc& desc, const TechniqueRecord& technique, MaterialRecord& record) = 0;
    virtual bool buildGeometry(const GeometryDesc& desc, GeometryRecord& record, GeometryAllocation& allocation) = 0;

    virtual void destroyTechnique(TechniqueRecord& record) = 0;
    virtual void destroyMaterial(MaterialRecord& record) = 0;
    virtual void destroyGeometry(GeometryRecord& record, GeometryAllocation& allocation) = 0;
};

} // namespace ember::vulkan