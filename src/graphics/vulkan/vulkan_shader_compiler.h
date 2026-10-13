#pragma once

#include <vulkan/vulkan.hpp>

#include "vulkan_resource_defs.h"

namespace ember::graphics::vulkan {

/**
 * @brief Compiles shader source via shaderc and creates Vulkan pipelines,
 * descriptor layouts, and geometry buffers.
 */
class VulkanShaderCompiler {
public:
    virtual ~VulkanShaderCompiler() = default;

    virtual bool compileShaderStage(const ShaderStageDesc& stageDesc, VkShaderModule& module) = 0;
    virtual void destroyShaderModule(VkShaderModule module) = 0;
};

} // namespace ember::graphics::vulkan