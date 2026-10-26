#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include <logger.h>

#include "vulkan_resource_defs.h"

namespace ember::vulkan {

class VulkanPipelineBuilder {
public:
	VulkanPipelineBuilder& withDevice(VkDevice device) {
		device_ = device;

		return *this;
	}

	VulkanPipelineBuilder& withBlendState(const VkPipelineColorBlendStateCreateInfo& blendState) {
		colorBlendState_ = blendState;
		return *this;
	}

	VulkanPipelineBuilder& withBlendAttachmentState(const VkPipelineColorBlendAttachmentState& attachmentState) {
		colorBlendAttachments_.push_back(attachmentState);
		return *this;
	}

	VulkanPipelineBuilder& withOpaqueBlendState() {
		VkPipelineColorBlendAttachmentState attachmentState{};
		attachmentState.blendEnable = VK_FALSE;
		attachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		
		return withBlendAttachmentState(attachmentState);
	}

	VulkanPipelineBuilder& withTransparentBlendState() {
		VkPipelineColorBlendAttachmentState attachmentState{};
		attachmentState.blendEnable = VK_TRUE;
		attachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		attachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		attachmentState.colorBlendOp = VK_BLEND_OP_ADD;
		attachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		attachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		attachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
		attachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		return withBlendAttachmentState(attachmentState);
	}

	VulkanPipelineBuilder& withDepthStencilState(const VkPipelineDepthStencilStateCreateInfo& depthStencilState) {
		depthStencilState_ = depthStencilState;
		return *this;
	}

	VulkanPipelineBuilder& withDepthTest(bool enable) {
		auto depthStencilState = getOrCreateDepthStencilState();
		depthStencilState.depthTestEnable = enable ? VK_TRUE : VK_FALSE;
		
		return withDepthStencilState(depthStencilState);
	}

	VulkanPipelineBuilder& withDepthWrite(bool enable) {
		auto depthStencilState = getOrCreateDepthStencilState();
		depthStencilState.depthWriteEnable = enable ? VK_TRUE : VK_FALSE;
		
		return withDepthStencilState(depthStencilState);
	}

	VulkanPipelineBuilder& withDepthTestAndWrite(bool enable) {
		auto depthStencilState = getOrCreateDepthStencilState();
		depthStencilState.depthTestEnable = enable ? VK_TRUE : VK_FALSE;
		depthStencilState.depthWriteEnable = enable ? VK_TRUE : VK_FALSE;
		
		return withDepthStencilState(depthStencilState);
	}

	VulkanPipelineBuilder& withShaderStage(const ShaderRecord& record) {
		withShaderStage(
			VkPipelineShaderStageCreateInfo{
					.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
					.stage = record.type,
					.module = record.module,
					.pName = "main" // Assuming entry point is "main"
			});

		return *this;
	}

	VulkanPipelineBuilder& withShaderStage(const VkPipelineShaderStageCreateInfo& shaderInfo) {
		shaderStages_.emplace_back(shaderInfo);

		if (shaderInfo.stage == VK_SHADER_STAGE_VERTEX_BIT) {
			hasVertex_ = true;
		}
		else if (shaderInfo.stage == VK_SHADER_STAGE_FRAGMENT_BIT) {
			hasFragment_ = true;
		}
		else if (shaderInfo.stage == VK_SHADER_STAGE_COMPUTE_BIT) {
			hasCompute_ = true;
		}

		return *this;
	}

	bool validate() {
		if (device_ == VK_NULL_HANDLE) {
			EMBER_LOG_ERROR("Pipeline builder requires a valid Vulkan device");
			return false;
		}

		for (auto& attachment : colorBlendAttachments_) {
			// Ensure that the attachment is valid
			if (attachment.colorWriteMask == 0) {
				EMBER_LOG_ERROR("Color blend attachment has no color write mask set");
				return false;
			}
		}

		if (shaderStages_.empty()) {
			EMBER_LOG_ERROR("Pipeline must have at least one shader stage");
			return false;
		}

		if (hasCompute_ && (hasVertex_ || hasFragment_)) {
			EMBER_LOG_ERROR("Cannot mix compute stage with graphics stages");
			return false;
		}
		if (!hasCompute_ && !hasVertex_) {
			EMBER_LOG_ERROR("Graphics pipeline requires at least a vertex stage");
			return false;
		}

		return true;
	}
	bool build(VkPipeline& pipeline) {
		if (!validate()) {
			pipeline = VK_NULL_HANDLE;

			return false;
		}
		
		VkGraphicsPipelineCreateInfo pipelineInfo{
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.stageCount = static_cast<uint32_t>(shaderStages_.size()),
			.pStages = shaderStages_.data()
		};

		if (!colorBlendAttachments_.empty()) {
			VkPipelineColorBlendStateCreateInfo colorBlendState = getOrCreateColorBlendState();
			colorBlendState.attachmentCount = static_cast<uint32_t>(colorBlendAttachments_.size());
			colorBlendState.pAttachments = colorBlendAttachments_.data();
			pipelineInfo.pColorBlendState = &colorBlendState;
		} else {
			pipelineInfo.pColorBlendState = colorBlendState_ ? &*colorBlendState_ : nullptr;
		}

		pipelineInfo.pDepthStencilState = depthStencilState_ ? &*depthStencilState_ : nullptr;

		vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);

		return true;
	}

private:
	VkPipelineColorBlendStateCreateInfo getOrCreateColorBlendState() {
		return colorBlendState_.value_or(VkPipelineColorBlendStateCreateInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.logicOpEnable = VK_FALSE,
			.attachmentCount = 0,
			.pAttachments = nullptr // No attachments by default
			});
	};

	VkPipelineDepthStencilStateCreateInfo getOrCreateDepthStencilState() {
		return depthStencilState_.value_or(VkPipelineDepthStencilStateCreateInfo{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.depthTestEnable = VK_TRUE,
			.depthWriteEnable = VK_TRUE,
			.depthCompareOp = VK_COMPARE_OP_LESS
		});
	};
	

	VkDevice device_ = VK_NULL_HANDLE;
	std::optional<VkPipelineColorBlendStateCreateInfo> colorBlendState_;
	std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments_;
	std::optional<VkPipelineDepthStencilStateCreateInfo> depthStencilState_;
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages_;
	bool hasVertex_ = false; 
	bool hasFragment_ = false; 
	bool hasCompute_ = false;
};

}