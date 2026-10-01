#pragma once

#include "vulkan_resource_compiler.h"
#include "vulkan_context.h"
#include <memory>

namespace ember::graphics::vulkan {

/**
 * @brief Compiles shader source via shaderc and creates Vulkan pipelines,
 * descriptor layouts, and geometry buffers.
 */
class ShadercVulkanResourceCompiler : public VulkanResourceCompiler {
public:
    explicit ShadercVulkanResourceCompiler(std::shared_ptr<VulkanContext> context);

    bool compileTechnique(const TechniqueDesc& desc, TechniqueRecord& outRecord) override;
    bool compileMaterial(const MaterialDesc& desc, const TechniqueRecord& technique, MaterialRecord& outRecord) override;
    bool compileGeometry(const GeometryDesc& desc, GeometryRecord& outRecord, GeometryAllocation& outAllocation) override;

    void destroyTechnique(TechniqueRecord& record) override;
    void destroyMaterial(MaterialRecord& record) override;
    void destroyGeometry(GeometryRecord& record, GeometryAllocation& allocation) override;

private:
    std::shared_ptr<VulkanContext> context_;
    // shaderc::Compiler instance, options, etc. kept private to this implementation
};

std::unique_ptr<VulkanResourceCompiler> createShadercVulkanResourceCompiler(std::shared_ptr<VulkanContext> context);

} // namespace ember::graphics::vulkan