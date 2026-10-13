#pragma once

#include "vulkan_resource_builder.h"

#include <memory>

#include <resource_file_provider.h>

#include "../context/vulkan_context.h"
#include "vulkan_shader_compiler.h"

namespace ember::graphics::vulkan {

/**
 * @brief Compiles shader source via shaderc and creates Vulkan pipelines,
 * descriptor layouts, and geometry buffers.
 */
class DefaultVulkanResourceBuilder : public VulkanResourceBuilder {
public:
    explicit DefaultVulkanResourceBuilder(
    	std::shared_ptr<VulkanContext> context,
        std::unique_ptr<VulkanShaderCompiler> shaderCompiler,
        std::shared_ptr<core::ResourceFileProvider> fileProvider);

    bool buildTechnique(const TechniqueDesc& desc, TechniqueRecord& technique) override;
    bool buildMaterial(const MaterialDesc& desc, const TechniqueRecord& technique, MaterialRecord& material) override;
    bool buildGeometry(const GeometryDesc& desc, GeometryRecord& geometry, GeometryAllocation& allocation) override;

    void destroyTechnique(TechniqueRecord& technique) override;
    void destroyMaterial(MaterialRecord& material) override;
    void destroyGeometry(GeometryRecord& geometry, GeometryAllocation& allocation) override;

private:
    static VkShaderStageFlagBits toShaderStageFlagBits(ShaderStage stage);

    std::shared_ptr<VulkanContext> context_;
    std::unique_ptr<VulkanShaderCompiler> shaderCompiler_;
    std::shared_ptr<core::ResourceFileProvider> fileProvider_;
};

std::unique_ptr<VulkanResourceBuilder> createDefaultVulkanResourceBuilder(std::shared_ptr<VulkanContext> context, std::unique_ptr<VulkanShaderCompiler> shaderCompiler, std::shared_ptr<core::ResourceFileProvider> fileProvider);
} // namespace ember::graphics::vulkan