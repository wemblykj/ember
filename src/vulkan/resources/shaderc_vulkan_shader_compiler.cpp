#include "shaderc_vulkan_shader_compiler.h"

#include <logger.h>

#include "cleanup_stack.h"

using namespace ember::core;
using namespace ember::resources;

namespace ember::vulkan {

ShadercVulkanShaderCompiler::ShadercVulkanShaderCompiler(
	const shaderc::CompileOptions& options, 
	std::shared_ptr<VulkanContext> context,
	std::shared_ptr<ResourceFileProvider> fileProvider)
    : options_(options)
    , context_(std::move(context))
    , fileProvider_(std::move(fileProvider)) {
	compiler_ = std::make_unique<shaderc::Compiler>();
}

bool ShadercVulkanShaderCompiler::compileShaderModule(const std::string& sourcePath, shaderc_shader_kind kind,
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

	if (!context_->createShaderModule(spvWords, shaderModule)) {
		EMBER_LOG_ERROR("vkCreateShaderModule failed for '{}'", sourcePath);
		return false;
	}

	return true;
}

bool ShadercVulkanShaderCompiler::compileShaderModule(const ShaderDesc& shaderDesc, VkShaderModule& module) {
	const auto kind = toShaderKind(shaderDesc.type);

	return compileShaderModule(shaderDesc.sourcePath, kind, module);
}

void ShadercVulkanShaderCompiler::destroyShaderModule(VkShaderModule module) {
	context_->destroyShaderModule(module);
}

shaderc_shader_kind ShadercVulkanShaderCompiler::toShaderKind(ShaderType type) {
	switch (type) {
	case ShaderType::Vertex: return shaderc_vertex_shader;
	case ShaderType::Fragment: return shaderc_fragment_shader;
	case ShaderType::Compute: return shaderc_compute_shader;
	case ShaderType::Geometry: return shaderc_geometry_shader;
	case ShaderType::TessellationControl: return shaderc_tess_control_shader;
	case ShaderType::TessellationEvaluation: return shaderc_tess_evaluation_shader;
	}

	throw std::runtime_error("Unsupported shader stage");
}

} // namespace ember::vulkan