#pragma once

#include <memory>

#include <shaderc/shaderc.hpp>

#include "resource_file_provider.h"
#include "vulkan_resource_compiler.h"
#include "vulkan_context.h"

namespace ember::graphics::vulkan {

/**
 * @brief Compiles shader source via shaderc and creates Vulkan pipelines,
 * descriptor layouts, and geometry buffers.
 */
class ShadercVulkanResourceCompiler : public VulkanResourceCompiler {
public:
    explicit ShadercVulkanResourceCompiler(
    	shaderc::CompileOptions& options, 
    	std::shared_ptr<VulkanContext> context,
        std::shared_ptr<core::ResourceFileProvider> fileProvider);

    bool compileTechnique(const TechniqueDesc& desc, TechniqueRecord& technique) override;
    bool compileMaterial(const MaterialDesc& desc, const TechniqueRecord& technique, MaterialRecord& material) override;
    bool compileGeometry(const GeometryDesc& desc, GeometryRecord& geometry, GeometryAllocation& allocation) override;

    void destroyTechnique(TechniqueRecord& technique) override;
    void destroyMaterial(MaterialRecord& material) override;
    void destroyGeometry(GeometryRecord& geometry, GeometryAllocation& allocation) override;

private:
    /// @brief Compiles a single shader stage from source into a VkShaderModule.
    /// Internal helper used by compileTechnique(); not part of the public interface.
    bool compileShaderStage(const std::string& sourcePath, shaderc_shader_kind kind, VkShaderModule& module);
    bool compileShaderStage(const ShaderStageDesc& stageDesc, VkShaderModule& module);
    void destroyShaderModule(VkShaderModule module);

    static shaderc_shader_kind toShaderKind(ShaderStage stage);
    static VkShaderStageFlagBits toShaderStageFlagBits(ShaderStage stage);

    std::shared_ptr<VulkanContext> context_;
	std::unique_ptr<shaderc::Compiler> compiler_;
	shaderc::CompileOptions options_;
    std::shared_ptr<core::ResourceFileProvider> fileProvider_;

    // shaderc::Compiler instance, options, etc. kept private to this implementation
};

std::unique_ptr<VulkanResourceCompiler> createShadercVulkanResourceCompiler(std::shared_ptr<VulkanContext> context);

} // namespace ember::graphics::vulkan