#include "shaderc_vulkan_resource_compiler.h"

#include <filesystem>

#include "cleanup_stack.h"

namespace ember::graphics::vulkan {

ShadercVulkanResourceCompiler::ShadercVulkanResourceCompiler(
	shaderc::CompileOptions& options, 
	std::shared_ptr<VulkanContext> context,
	std::shared_ptr<core::ResourceFileProvider> fileProvider)
    : options_(options)
    , context_(std::move(context))
    , fileProvider_(std::move(fileProvider)) {
	compiler_ = std::make_unique<shaderc::Compiler>();
}

bool ShadercVulkanResourceCompiler::compileTechnique(const TechniqueDesc& desc, TechniqueRecord& technique) {
	bool hasVertex = false, hasFragment = false, hasCompute = false;

	core::CleanupStack cleanup;

	std::vector<VkPipelineShaderStageCreateInfo> stages;

	for (const auto& stage : desc.shaderStages) {
		const auto kind = toShaderKind(stage.stage);

		VkShaderModule module = VK_NULL_HANDLE;
		if (!compileShaderStage(stage.sourcePath, kind, module)) {
			EMBER_LOG_ERROR("Technique '{}': failed to compile shader stage '{}'", desc.name, stage.sourcePath);
			return false;
		}

		cleanup.push([&, module] { destroyShaderModule(module); });

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

bool ShadercVulkanResourceCompiler::compileMaterial(const MaterialDesc& desc, const TechniqueRecord& technique,
	MaterialRecord& material) {
	throw std::runtime_error("Not implemented");
}

bool ShadercVulkanResourceCompiler::compileGeometry(const GeometryDesc& desc, GeometryRecord& geometry,
	GeometryAllocation& outAllocation) {
	throw std::runtime_error("Not implemented");
}

void ShadercVulkanResourceCompiler::destroyTechnique(TechniqueRecord& record) {
	throw std::runtime_error("Not implemented");
}

void ShadercVulkanResourceCompiler::destroyMaterial(MaterialRecord& material) {
	throw std::runtime_error("Not implemented");
}

void ShadercVulkanResourceCompiler::destroyGeometry(GeometryRecord& geometry, GeometryAllocation& allocation) {
	throw std::runtime_error("Not implemented");
}

bool ShadercVulkanResourceCompiler::compileShaderStage(const std::string& sourcePath, shaderc_shader_kind kind,
                                                       VkShaderModule& shaderModule) {
	auto source = fileProvider_->readText(sourcePath);
	if (!source) {
		EMBER_LOG_ERROR("Failed to read shader source '{}'", sourcePath);
		return false;
	}

	const auto module = compiler_->CompileGlslToSpv(*source, kind, sourcePath.c_str(), options_);

	if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
		EMBER_LOG_ERROR("Shader compilation failed for '{}': {}", sourcePath, module.GetErrorMessage());
		return false;
	}

	std::span spvWords(module.cbegin(), module.cend());

	if (context_->createShaderModule(spvWords, shaderModule) != VK_SUCCESS) {
		EMBER_LOG_ERROR("vkCreateShaderModule failed for '{}'", sourcePath);
		return false;
	}

	return true;
}

bool ShadercVulkanResourceCompiler::compileShaderStage(const ShaderStageDesc& stageDesc, VkShaderModule& module) {
	const auto kind = toShaderKind(stageDesc.stage);

	return compileShaderStage(stageDesc.sourcePath, kind, module);
}

void ShadercVulkanResourceCompiler::destroyShaderModule(VkShaderModule module) {
	context_->destroyShaderModule(module);
}

shaderc_shader_kind ShadercVulkanResourceCompiler::toShaderKind(ShaderStage stage) {
	switch (stage) {
	case ShaderStage::Vertex: return shaderc_vertex_shader;
	case ShaderStage::Fragment: return shaderc_fragment_shader;
	case ShaderStage::Compute: return shaderc_compute_shader;
	case ShaderStage::Geometry: return shaderc_geometry_shader;
	case ShaderStage::TessellationControl: return shaderc_tess_control_shader;
	case ShaderStage::TessellationEvaluation: return shaderc_tess_evaluation_shader;
	}

	throw std::runtime_error("Unsupported shader stage");
}

VkShaderStageFlagBits ShadercVulkanResourceCompiler::toShaderStageFlagBits(ShaderStage stage) {
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

} // namespace ember::graphics::vulkan