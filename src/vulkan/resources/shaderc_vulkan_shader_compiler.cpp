#include "shaderc_vulkan_shader_compiler.h"

#include <filesystem>

#include "cleanup_stack.h"

namespace ember::graphics::vulkan {

ShadercVulkanShaderCompiler::ShadercVulkanShaderCompiler(
	const shaderc::CompileOptions& options, 
	std::shared_ptr<VulkanContext> context,
	std::shared_ptr<core::ResourceFileProvider> fileProvider)
    : options_(options)
    , context_(std::move(context))
    , fileProvider_(std::move(fileProvider)) {
	compiler_ = std::make_unique<shaderc::Compiler>();
}

bool ShadercVulkanShaderCompiler::compileShaderStage(const std::string& sourcePath, shaderc_shader_kind kind,
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

bool ShadercVulkanShaderCompiler::compileShaderStage(const ShaderStageDesc& stageDesc, VkShaderModule& module) {
	const auto kind = toShaderKind(stageDesc.stage);

	return compileShaderStage(stageDesc.sourcePath, kind, module);
}

void ShadercVulkanShaderCompiler::destroyShaderModule(VkShaderModule module) {
	context_->destroyShaderModule(module);
}

shaderc_shader_kind ShadercVulkanShaderCompiler::toShaderKind(ShaderStage stage) {
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

} // namespace ember::graphics::vulkan