#include "default_vulkan_resource_builder.h"

#include <logger.h>

#include "cleanup_stack.h"
#include "vulkan_pipeline_builder.h"

using namespace ember::core;
using namespace ember::resources;

namespace ember::vulkan {

DefaultVulkanResourceBuilder::DefaultVulkanResourceBuilder(
	std::shared_ptr<VulkanContext> context,
	std::unique_ptr<VulkanShaderCompiler> shaderCompiler,
	std::shared_ptr<ResourceFileProvider> fileProvider)
	: context_(std::move(context))
	, shaderCompiler_(std::move(shaderCompiler))
	, fileProvider_(std::move(fileProvider)) {
}

bool DefaultVulkanResourceBuilder::buildCannedShader(ShaderID id, ShaderRecord& record) {
	return false;
}

bool DefaultVulkanResourceBuilder::buildCannedTechnique(TechniqueID id, const VulkanResourceResolver& resourceResolver, TechniqueRecord& technique) {
	switch (id) {
	case BuiltinTechnique::Opaque:
		return buildOpaqueTechnique(resourceResolver, technique);
	case BuiltinTechnique::Transparent:
		return buildTransparentTechnique(resourceResolver, technique);
	}

	return false;
}

bool DefaultVulkanResourceBuilder::buildCannedMaterial(MaterialID id, MaterialRecord& record) {
	return false;
}

bool DefaultVulkanResourceBuilder::buildCannedGeometry(GeometryID id, GeometryRecord& record) {
	return false;
}

bool DefaultVulkanResourceBuilder::buildCannedVertexFormat(VertexFormatID id, VertexFormatRecord& record) {
	return false;
}

bool DefaultVulkanResourceBuilder::buildShader(const ShaderDesc& desc, ShaderRecord& record) {
	VkShaderModule module = VK_NULL_HANDLE;
	if (!shaderCompiler_->compileShaderModule(desc, module)) {
		EMBER_LOG_ERROR("Technique '{}': failed to compile shader stage '{}'", desc.name, desc.sourcePath);
		return false;
	}

	record.module = module;
	record.type = toShaderStageFlagBits(desc.type);

	return true;
}

bool DefaultVulkanResourceBuilder::buildTechnique(const TechniqueDesc& desc, const VulkanResourceResolver& resourceResolver, TechniqueRecord& technique) {
	VulkanPipelineBuilder builder;

	builder
		.withShaderStage(resourceResolver.resolveShader(desc.shaderStages.vertex))
		.withShaderStage(resourceResolver.resolveShader(desc.shaderStages.fragment))
		.withShaderStage(resourceResolver.resolveShader(desc.shaderStages.compute));

	builder.validate();

	return builder
		.withDevice(context_->getDevice())
		.build(technique.pipeline);
}

bool DefaultVulkanResourceBuilder::buildMaterial(const MaterialDesc& desc, MaterialRecord& material) {
	//const auto technique = resourceResolver->resolveTechnique(desc.technique);

	throw std::runtime_error("Not implemented");
}

bool DefaultVulkanResourceBuilder::buildGeometry(const GeometryDesc& desc, GeometryRecord& geometry,
	GeometryAllocation& allocation) {
	throw std::runtime_error("Not implemented");
}

bool DefaultVulkanResourceBuilder::buildVertexFormat(const VertexFormatDesc& desc, VertexFormatRecord& record) {
	return false;
}

void DefaultVulkanResourceBuilder::destroyShader(ShaderRecord& record) {
	if (record.module != VK_NULL_HANDLE) {
		shaderCompiler_->destroyShaderModule(record.module);
		record.module = VK_NULL_HANDLE;
	}
}

void DefaultVulkanResourceBuilder::destroyTechnique(TechniqueRecord& technique) {
	vkDestroyPipeline(context_->getDevice(), technique.pipeline, nullptr);
	technique.pipeline = VK_NULL_HANDLE;
}

void DefaultVulkanResourceBuilder::destroyMaterial(MaterialRecord& material) {
	throw std::runtime_error("Not implemented");
}

void DefaultVulkanResourceBuilder::destroyGeometry(GeometryRecord& geometry, GeometryAllocation& allocation) {
	throw std::runtime_error("Not implemented");
}

void DefaultVulkanResourceBuilder::destroyVertexFormat(VertexFormatRecord& record) {

}

bool DefaultVulkanResourceBuilder::buildOpaqueTechnique(const VulkanResourceResolver& resourceResolver, TechniqueRecord& technique) {
	VulkanPipelineBuilder pipelineBuilder;

	pipelineBuilder
		.withShaderStage(resourceResolver.resolveShader(BuiltinShader::GenericTransform))
		.withShaderStage(resourceResolver.resolveShader(BuiltinShader::OpaquePBR))
		.withDevice(context_->getDevice())
		.build(technique.pipeline);
	
	return true;
}

bool DefaultVulkanResourceBuilder::buildTransparentTechnique(const VulkanResourceResolver& resourceResolver, TechniqueRecord& technique) {
	VulkanPipelineBuilder pipelineBuilder;

	pipelineBuilder
		.withShaderStage(resourceResolver.resolveShader(BuiltinShader::GenericTransform))
		.withShaderStage(resourceResolver.resolveShader(BuiltinShader::OpaquePBR))
		.withDevice(context_->getDevice())
		.build(technique.pipeline);

	return true;
}

VkShaderStageFlagBits DefaultVulkanResourceBuilder::toShaderStageFlagBits(ShaderType type) {
	switch (type) {
	case ShaderType::Vertex: return VK_SHADER_STAGE_VERTEX_BIT;
	case ShaderType::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
	case ShaderType::Compute: return VK_SHADER_STAGE_COMPUTE_BIT;
	case ShaderType::Geometry: return VK_SHADER_STAGE_GEOMETRY_BIT;
	case ShaderType::TessellationControl: return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	case ShaderType::TessellationEvaluation: return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	}

	throw std::runtime_error("Unsupported shader stage");
}

} // namespace ember::vulkan