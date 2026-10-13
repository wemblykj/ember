#include "default_vulkan_resource_builder.h"

#include "cleanup_stack.h"

using namespace ember::core;
using namespace ember::resources;

namespace ember::vulkan {

DefaultVulkanResourceBuilder::DefaultVulkanResourceBuilder(
	std::shared_ptr<VulkanContext> context,
	std::unique_ptr<VulkanShaderCompiler> shaderCompiler,
	std::shared_ptr<ResourceFileProvider> fileProvider)
    : context_(std::move(context))
    , shaderCompiler_(std::move(shaderCompiler	))
    , fileProvider_(std::move(fileProvider)) {
}

bool DefaultVulkanResourceBuilder::buildTechnique(const TechniqueDesc& desc, TechniqueRecord& technique) {
	bool hasVertex = false, hasFragment = false, hasCompute = false;

	CleanupStack cleanup;

	std::vector<VkPipelineShaderStageCreateInfo> stages;

	for (const auto& stage : desc.shaderStages) {
		VkShaderModule module = VK_NULL_HANDLE;
		if (!shaderCompiler_->compileShaderStage(stage, module)) {
			EMBER_LOG_ERROR("Technique '{}': failed to compile shader stage '{}'", desc.name, stage.sourcePath);
			return false;
		}

		cleanup.push([&, module] { shaderCompiler_->destroyShaderModule(module); });

		switch (stage.stage) {
		case ShaderStage::Vertex:   technique.vertModule = module; hasVertex = true; break;
		case ShaderStage::Fragment: technique.fragModule = module; hasFragment = true; break;
		case ShaderStage::Compute:  technique.computeModule = module; hasCompute = true; break;
			// ...
		}

		VkPipelineShaderStageCreateInfo shaderStageInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = toShaderStageFlagBits(stage.stage),
			.module = module,
			.pName = stage.entryPoint.c_str()
		};

		stages.emplace_back(shaderStageInfo);
	}

	if (hasCompute && (hasVertex || hasFragment)) {
		EMBER_LOG_ERROR("Technique '{}': cannot mix compute stage with graphics stages", desc.name);
		return false;
	}
	if (!hasCompute && !hasVertex) {
		EMBER_LOG_ERROR("Technique '{}': graphics techniques require at least a vertex stage", desc.name);
		return false;
	}

	VkGraphicsPipelineCreateInfo pipelineInfo{};

	pipelineInfo.stageCount = static_cast<uint32_t>(stages.size());
	pipelineInfo.pStages = stages.data();

	vkCreateGraphicsPipelines(context_->getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &technique.pipeline);

	cleanup.dismissAll();

	return true;
}

bool DefaultVulkanResourceBuilder::buildMaterial(const MaterialDesc& desc, const TechniqueRecord& technique,
	MaterialRecord& material) {
	throw std::runtime_error("Not implemented");
}

bool DefaultVulkanResourceBuilder::buildGeometry(const GeometryDesc& desc, GeometryRecord& geometry,
	GeometryAllocation& outAllocation) {
	throw std::runtime_error("Not implemented");
}

void DefaultVulkanResourceBuilder::destroyTechnique(TechniqueRecord& technique) {
	if (technique.fragModule == VK_NULL_HANDLE) {
		shaderCompiler_->destroyShaderModule(technique.fragModule);
		technique.fragModule = VK_NULL_HANDLE;
	}

	if (technique.vertModule == VK_NULL_HANDLE) {
		shaderCompiler_->destroyShaderModule(technique.vertModule);
		technique.vertModule = VK_NULL_HANDLE;
	}

	if (technique.computeModule == VK_NULL_HANDLE) {
		shaderCompiler_->destroyShaderModule(technique.computeModule);
		technique.computeModule = VK_NULL_HANDLE;
	}
}

void DefaultVulkanResourceBuilder::destroyMaterial(MaterialRecord& material) {
	throw std::runtime_error("Not implemented");
}

void DefaultVulkanResourceBuilder::destroyGeometry(GeometryRecord& geometry, GeometryAllocation& allocation) {
	throw std::runtime_error("Not implemented");
}

VkShaderStageFlagBits DefaultVulkanResourceBuilder::toShaderStageFlagBits(ShaderStage stage) {
	switch (stage) {
	case ShaderStage::Vertex: return VK_SHADER_STAGE_VERTEX_BIT;
	case ShaderStage::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
	case ShaderStage::Compute: return VK_SHADER_STAGE_COMPUTE_BIT;
	case ShaderStage::Geometry: return VK_SHADER_STAGE_GEOMETRY_BIT;
	case ShaderStage::TessellationControl: return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	case ShaderStage::TessellationEvaluation: return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	}

	throw std::runtime_error("Unsupported shader stage");
}

} // namespace ember::vulkan