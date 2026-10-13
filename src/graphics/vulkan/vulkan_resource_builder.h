#pragma once

#include "vulkan_resource_defs.h"

namespace ember::graphics::vulkan {

/**
 * @brief Compiles backend-agnostic resource descriptions into Vulkan GPU objects.
 *
 * Implementations handle shader compilation (e.g. via shaderc), pipeline
 * creation, descriptor layout generation, and buffer/image uploads. The
 * resource cache calls this interface during registration; it does not
 * perform compilation itself.
 */
class VulkanResourceBuilder {
public:
    virtual ~VulkanResourceBuilder() = default;

    virtual bool compileTechnique(const TechniqueDesc& desc, TechniqueRecord& outRecord) = 0;
    virtual bool compileMaterial(const MaterialDesc& desc, const TechniqueRecord& technique, MaterialRecord& outRecord) = 0;
    virtual bool compileGeometry(const GeometryDesc& desc, GeometryRecord& outRecord, GeometryAllocation& allocation) = 0;

    virtual void destroyTechnique(TechniqueRecord& record) = 0;
    virtual void destroyMaterial(MaterialRecord& record) = 0;
    virtual void destroyGeometry(GeometryRecord& record, GeometryAllocation& allocation) = 0;
};

} // namespace ember::graphics::vulkan