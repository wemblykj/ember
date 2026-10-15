#pragma once

#include "vulkan_resource_builder.h"

#include <memory>

#include <resource_types.h>
#include <resource_file_provider.h>
#include <canned_resource_provider.h>

#include "context/vulkan_context.h"
#include "vulkan_shader_compiler.h"

namespace ember::vulkan {

/**
 * @brief Compiles shader source via shaderc and creates Vulkan pipelines,
 * descriptor layouts, and geometry buffers.
 */
class DefaultVulkanResourceBuilder : public VulkanResourceBuilder {
public:
    explicit DefaultVulkanResourceBuilder(
    	std::shared_ptr<VulkanContext> context,
        std::unique_ptr<VulkanShaderCompiler> shaderCompiler,
        std::unique_ptr<resources::CannedResourceProvider> cannedResourceProvider,
        std::shared_ptr<resources::ResourceFileProvider> fileProvider);

    bool buildCannedTechnique(TechniqueID id, TechniqueRecord& technique) override;
    bool buildCannedMaterial(MaterialID id, MaterialRecord& record) override;
    bool buildCannedGeometry(GeometryID id, GeometryRecord& record) override;
    bool buildCannedVertexFormat(VertexFormatID id, VertexFormatRecord& record) override;

    bool buildTechnique(const TechniqueDesc& desc, TechniqueRecord& technique) override;
    bool buildMaterial(const MaterialDesc& desc, const TechniqueRecord& technique, MaterialRecord& material) override;
    bool buildGeometry(const GeometryDesc& desc, GeometryRecord& geometry, GeometryAllocation& allocation) override;
	bool buildVertexFormat(const VertexFormatDesc& desc, VertexFormatRecord& record) override;

    void destroyTechnique(TechniqueRecord& technique) override;
    void destroyMaterial(MaterialRecord& material) override;
    void destroyGeometry(GeometryRecord& geometry, GeometryAllocation& allocation) override;
	void destroyVertexFormat(VertexFormatRecord& record) override;

private:
    bool buildOpaqueTechnique(const TechniqueRecord& technique);
    bool buildTransparentTechnique(const TechniqueRecord& technique);

    static VkShaderStageFlagBits toShaderStageFlagBits(resources::ShaderStage stage);

    std::shared_ptr<VulkanContext> context_;
    std::unique_ptr<VulkanShaderCompiler> shaderCompiler_;
	std::unique_ptr<resources::CannedResourceProvider> cannedResourceProvider_;
    std::shared_ptr<resources::ResourceFileProvider> fileProvider_;
};

std::unique_ptr<VulkanResourceBuilder> createDefaultVulkanResourceBuilder(std::shared_ptr<VulkanContext> context, std::unique_ptr<VulkanShaderCompiler> shaderCompiler, std::shared_ptr<resources::ResourceFileProvider> fileProvider);

} // namespace ember::vulkan