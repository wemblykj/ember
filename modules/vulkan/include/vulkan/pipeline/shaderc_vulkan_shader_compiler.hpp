#pragma once

#include "vulkan_shader_compiler.hpp"

#include <memory>

#include <shaderc/shaderc.hpp>

#include <resources/resource_file_provider.hpp>

#include <vulkan/context/vulkan_context.hpp>

namespace ember::vulkan {

/**
 * @brief Compiles shader source via shaderc and creates Vulkan pipelines,
 * descriptor layouts, and geometry buffers.
 */
class ShadercVulkanShaderCompiler : public VulkanShaderCompiler {
public:
    explicit ShadercVulkanShaderCompiler(
    	const shaderc::CompileOptions& options, 
    	std::shared_ptr<VulkanContext> context,
        std::shared_ptr<resources::ResourceFileProvider> fileProvider);

    bool compileShaderModule(const resources::ShaderDesc& stageDesc, VkShaderModule& module) override;
    void destroyShaderModule(VkShaderModule module) override;

private:
    bool compileShaderModule(const std::string& sourcePath, shaderc_shader_kind kind, VkShaderModule& module);

    static shaderc_shader_kind toShaderKind(resources::ShaderType type);

    shaderc::CompileOptions options_;
    std::shared_ptr<VulkanContext> context_;
    std::shared_ptr<resources::ResourceFileProvider> fileProvider_;
	std::unique_ptr<shaderc::Compiler> compiler_;
    
    // shaderc::Compiler instance, options, etc. kept private to this implementation
};

std::unique_ptr<VulkanShaderCompiler> createShadercVulkanShaderCompiler(const shaderc::CompileOptions& options, std::shared_ptr<VulkanContext> context);

} // namespace ember::vulkan