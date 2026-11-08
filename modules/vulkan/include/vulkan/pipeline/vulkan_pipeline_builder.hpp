#pragma once

#include <memory>
#include <span>
#include <vector>

#include <vulkan/vulkan.hpp>

#include <core/logger.hpp>
#include <core/action.hpp>

#include <vulkan/resources/vulkan_resource_defs.hpp>

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

	VkPipeline build() const;

private:	
	std::vector<VkPipelineShaderStageCreateInfo> buildShaderStages() const;	
	std::unique_ptr<VkPipelineVertexInputStateCreateInfo> buildVertexInputState() const;
	std::unique_ptr<VkPipelineViewportStateCreateInfo> buildViewportState() const;
	std::unique_ptr<VkPipelineRasterizationStateCreateInfo> buildRasterizationState() const;
	std::unique_ptr<VkPipelineMultisampleStateCreateInfo> buildMultisampleState() const;
	std::unique_ptr<VkPipelineInputAssemblyStateCreateInfo> buildInputAssemblyState() const;
	std::unique_ptr<VkPipelineDepthStencilStateCreateInfo> buildDepthStencilState() const;
	std::vector<VkPipelineColorBlendAttachmentState> buildColorBlendAttachmentStates() const;
	std::unique_ptr<VkPipelineColorBlendStateCreateInfo> buildColorBlendState(const std::span<VkPipelineColorBlendAttachmentState> attachmentStates) const;
	std::unique_ptr<VkPipelineDynamicStateCreateInfo> buildDynamicState() const;

	bool validateFundamentals() const;
	static bool validateShaderStages(const std::span<VkPipelineShaderStageCreateInfo>& shaderStages);
	static bool validateVertexInputState(const VkPipelineVertexInputStateCreateInfo& vertexInputState);
	static bool validateViewportState(const VkPipelineViewportStateCreateInfo& viewportState);
	static bool validateRasterizationState(const VkPipelineRasterizationStateCreateInfo& rasterizationState);
	static bool validateMultisampleState(const VkPipelineMultisampleStateCreateInfo& multisampleState);
	static bool validateInputAssemblyState(const VkPipelineInputAssemblyStateCreateInfo& inputAssemblyState);
	static bool validateDepthStencilState(const VkPipelineDepthStencilStateCreateInfo& depthStencilState);
	static bool validateColorBlendAttachmentState(const VkPipelineColorBlendAttachmentState& attachmentState);
	static bool validateColorBlendAttachmentStates(const std::span<VkPipelineColorBlendAttachmentState>& attachmentStates);
	static bool validateColorBlendState(const VkPipelineColorBlendStateCreateInfo& colorBlendState);
	static bool validateDynamicState(const VkPipelineDynamicStateCreateInfo& dynamicState);
	
	VkDevice device_ = VK_NULL_HANDLE;
	core::Action<VkPipelineColorBlendStateCreateInfo&> configureColorBlendState_;
	std::vector<core::Action<VkPipelineColorBlendAttachmentState&>> configureColorBlendAttachments_;
	core::Action<VkPipelineDepthStencilStateCreateInfo&> configureDepthStencilState_;
	std::vector<core::Action<VkPipelineShaderStageCreateInfo&>> configureShaderStages_;
};

}