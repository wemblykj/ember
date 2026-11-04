#pragma once

#include <vulkan/vulkan.hpp>

#include <resource_types.h>

using ShaderDesc = ember::resources::ShaderDesc;

namespace ember::vulkan {

/**
 * @brief Compiles shader source via shaderc and creates Vulkan pipelines,
 * descriptor layouts, and geometry buffers.
 */
class VulkanShaderCompiler {
public:
    virtual ~VulkanShaderCompiler() = default;

    virtual bool compileShaderModule(const ShaderDesc& stageDesc, VkShaderModule& module) = 0;
    virtual void destroyShaderModule(VkShaderModule module) = 0;
};

} // namespace ember::vulkan