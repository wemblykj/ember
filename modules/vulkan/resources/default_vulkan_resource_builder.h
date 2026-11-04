#pragma once

#include "vulkan_resource_builder.h"

#include <memory>

#include <resource_types.h>
#include <resource_file_provider.h>

#include "context/vulkan_context.h"

#include "vulkan_resource_resolver.h"
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
        std::shared_ptr<resources::ResourceFileProvider> fileProvider);

	bool buildCannedShader(ShaderID id, ShaderRecord& record) override;
    bool buildCannedTechnique(TechniqueID id, const VulkanResourceResolver& resourceResolver, TechniqueRecord& technique) override;
    bool buildCannedMaterial(MaterialID id, MaterialRecord& record) override;
    bool buildCannedGeometry(GeometryID id, GeometryRecord& record) override;
    bool buildCannedVertexFormat(VertexFormatID id, VertexFormatRecord& record) override;

	bool buildShader(const ShaderDesc& desc, ShaderRecord& record) override;
    bool buildTechnique(const TechniqueDesc& desc, const VulkanResourceResolver& resourceResolver, TechniqueRecord& technique) override;
    bool buildMaterial(const MaterialDesc& desc, MaterialRecord& material) override;
    bool buildGeometry(const GeometryDesc& desc, GeometryRecord& geometry, GeometryAllocation& allocation) override;
	bool buildVertexFormat(const VertexFormatDesc& desc, VertexFormatRecord& record) override;

	void destroyShader(ShaderRecord& record) override;
    void destroyTechnique(TechniqueRecord& technique) override;
    void destroyMaterial(MaterialRecord& material) override;
    void destroyGeometry(GeometryRecord& geometry, GeometryAllocation& allocation) override;
	void destroyVertexFormat(VertexFormatRecord& record) override;

private:
    bool buildPipeline(const resources::ShaderStages& shaderStages, const VulkanResourceResolver& resourceResolver, VkPipeline& pipeline);
    bool buildOpaqueTechnique(const VulkanResourceResolver& resourceResolver, TechniqueRecord& technique);
    bool buildTransparentTechnique(const VulkanResourceResolver& resourceResolver, TechniqueRecord& technique);

    static VkShaderStageFlagBits toShaderStageFlagBits(resources::ShaderType type);

    std::shared_ptr<VulkanContext> context_;
    std::unique_ptr<VulkanShaderCompiler> shaderCompiler_;
    std::shared_ptr<resources::ResourceFileProvider> fileProvider_;
};

std::unique_ptr<VulkanResourceBuilder> createDefaultVulkanResourceBuilder(std::shared_ptr<VulkanContext> context, std::unique_ptr<VulkanShaderCompiler> shaderCompiler, std::shared_ptr<resources::ResourceFileProvider> fileProvider);

} // namespace ember::vulkan