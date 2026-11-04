#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include <action.h>

#include "vulkan_resource_defs.h"

namespace ember::vulkan {

class VulkanPipelineBuilder {
public:
	VulkanPipelineBuilder& withDevice(VkDevice device);
	VulkanPipelineBuilder& configureColorBlendState(core::Action<VkPipelineColorBlendStateCreateInfo&>::Callback configure);
	VulkanPipelineBuilder& addBlendAttachmentState(core::Action<VkPipelineColorBlendAttachmentState&>::Callback configure);
	VulkanPipelineBuilder& addOpaqueBlendState(core::Action<VkPipelineColorBlendAttachmentState&>::OptionalCallback configure = {});
	VulkanPipelineBuilder& addTransparentBlendState(core::Action<VkPipelineColorBlendAttachmentState&>::OptionalCallback configure = {});
	VulkanPipelineBuilder& configureDepthStencilState(core::Action<VkPipelineDepthStencilStateCreateInfo&>::Callback configure);
	VulkanPipelineBuilder& withDepthTest(bool enable);
	VulkanPipelineBuilder& withDepthWrite(bool enable);
	VulkanPipelineBuilder& withDepthTestAndWrite(bool enable);
	VulkanPipelineBuilder& addShaderStage(core::Action<VkPipelineShaderStageCreateInfo&>::Callback configure);
	VulkanPipelineBuilder& withShaderStage(const ShaderRecord& record);

	VkPipeline build();

private:	
	bool validateShaderStages(const std::span<VkPipelineShaderStageCreateInfo>& shaderStages);
	bool validateFundamentals();
	bool validateColorBlendAttachmentState(const VkPipelineColorBlendAttachmentState& attachmentState);
	bool validateColorBlendState(const VkPipelineColorBlendStateCreateInfo& colorBlendState);
	bool validateColorBlendAttachmentStates(const std::span<VkPipelineColorBlendAttachmentState>& attachmentStates);
	bool validateDepthStencilState(const VkPipelineDepthStencilStateCreateInfo& depthStencilState);

	VkDevice device_ = VK_NULL_HANDLE;
	core::Action<VkPipelineColorBlendStateCreateInfo&> configureColorBlendState_;
	std::vector<core::Action<VkPipelineColorBlendAttachmentState&>> configureColorBlendAttachments_;
	core::Action<VkPipelineDepthStencilStateCreateInfo&> configureDepthStencilState_;
	std::vector<core::Action<VkPipelineShaderStageCreateInfo&>> configureShaderStages_;
};

}