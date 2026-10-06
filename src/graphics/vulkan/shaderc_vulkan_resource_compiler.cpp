#include "shaderc_vulkan_resource_compiler.h"

#include <filesystem>
#include <fstream>

namespace ember::graphics::vulkan {

ShadercVulkanResourceCompiler::ShadercVulkanResourceCompiler(
	shaderc::CompileOptions& options, 
	std::shared_ptr<VulkanContext> context,
	std::shared_ptr<core::ResourceFileProvider> fileProvider)
    : options_(options)
    , context_(std::move(context))
    , fileProvider_(std::move(fileProvider))
{
	compiler_ = std::make_unique<shaderc::Compiler>();
}

bool ShadercVulkanResourceCompiler::compileTechnique(const TechniqueDesc& desc, TechniqueRecord& technique)
{
	shaderc_shader_kind kind = shaderc_fragment_shader;

	auto source = fileProvider_->readText(desc.fragmentShader.sourcePath);
	if (!source) {
		EMBER_LOG_ERROR("Failed to read shader source '{}'", desc.fragmentShader.sourcePath);
		return false;
	}

	const auto module = compiler_->CompileGlslToSpv(*source, kind, desc.fragmentShader.sourcePath.c_str(), options_);

	if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
		EMBER_LOG_ERROR("Shader compilation failed for '{}': {}", desc.fragmentShader.sourcePath, module.GetErrorMessage());
		return false;
	}

	std::span spirvWords(module.cbegin(), module.cend());

	VkShaderModule shaderModule = VK_NULL_HANDLE;
	if (context_->createShaderModule(spirvWords, shaderModule) != VK_SUCCESS) {
		EMBER_LOG_ERROR("vkCreateShaderModule failed for '{}'", desc.fragmentShader.sourcePath);
		return false;
	}

	technique.fragModule = shaderModule;

	return true;
	
	return true;
}

bool ShadercVulkanResourceCompiler::compileMaterial(const MaterialDesc& desc, const TechniqueRecord& technique,
	MaterialRecord& material)
{
	throw std::runtime_error("Not implemented");
}

bool ShadercVulkanResourceCompiler::compileGeometry(const GeometryDesc& desc, GeometryRecord& geometry,
	GeometryAllocation& outAllocation)
{
	throw std::runtime_error("Not implemented");
}

void ShadercVulkanResourceCompiler::destroyTechnique(TechniqueRecord& record)
{
	throw std::runtime_error("Not implemented");
}

void ShadercVulkanResourceCompiler::destroyMaterial(MaterialRecord& material)
{
	throw std::runtime_error("Not implemented");
}

void ShadercVulkanResourceCompiler::destroyGeometry(GeometryRecord& geometry, GeometryAllocation& allocation)
{
	throw std::runtime_error("Not implemented");
}

} // namespace ember::graphics::vulkan