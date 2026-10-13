#pragma once

#include "vulkan_shader_compiler.h"

#include <memory>

#include <shaderc/shaderc.hpp>

#include <resource_file_provider.h>

#include "context/vulkan_context.h"

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

    bool compileShaderStage(const resources::ShaderStageDesc& stageDesc, VkShaderModule& module) override;
    void destroyShaderModule(VkShaderModule module) override;

private:
    bool compileShaderStage(const std::string& sourcePath, shaderc_shader_kind kind, VkShaderModule& module);

    static shaderc_shader_kind toShaderKind(resources::ShaderStage stage);

    shaderc::CompileOptions options_;
    std::shared_ptr<VulkanContext> context_;
    std::shared_ptr<resources::ResourceFileProvider> fileProvider_;
	std::unique_ptr<shaderc::Compiler> compiler_;
    
    // shaderc::Compiler instance, options, etc. kept private to this implementation
};

std::unique_ptr<VulkanShaderCompiler> createShadercVulkanShaderCompiler(const shaderc::CompileOptions& options, std::shared_ptr<VulkanContext> context);

} // namespace ember::vulkan