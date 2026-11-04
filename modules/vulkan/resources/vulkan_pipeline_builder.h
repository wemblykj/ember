#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include <logger.h>

#include "vulkan_resource_defs.h"
#include <action.h>

namespace ember::vulkan {

class VulkanPipelineBuilder {
public:
	VulkanPipelineBuilder& withDevice(VkDevice device) {
		device_ = device;

		return *this;
	}

	VulkanPipelineBuilder& configureColorBlendState(core::Action<VkPipelineColorBlendStateCreateInfo&>::Callback configure) {
		configureColorBlendState_ += configure;

		return *this;
	}

	VulkanPipelineBuilder& addBlendAttachmentState(core::Action<VkPipelineColorBlendAttachmentState&>::Callback configure) {
		configureColorBlendAttachments_.emplace_back() += configure;

		return *this;
	}

	VulkanPipelineBuilder& addOpaqueBlendState(core::Action<VkPipelineColorBlendAttachmentState&>::OptionalCallback configure = {}) {
		configureColorBlendState([](VkPipelineColorBlendStateCreateInfo& blendState) {
			blendState.logicOpEnable = VK_FALSE;
			blendState.logicOp = VK_LOGIC_OP_COPY;
			});

		addBlendAttachmentState([&configure](VkPipelineColorBlendAttachmentState& attachmentState) {
			attachmentState.blendEnable = VK_FALSE;
			attachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		
			configure.invoke(attachmentState);
			});
		
		return *this;
	}

	VulkanPipelineBuilder& addTransparentBlendState(core::Action<VkPipelineColorBlendAttachmentState&>::OptionalCallback configure = {}) {
		return addBlendAttachmentState([&configure](VkPipelineColorBlendAttachmentState& attachmentState) {
			attachmentState.blendEnable = VK_TRUE;
			attachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			attachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			attachmentState.colorBlendOp = VK_BLEND_OP_ADD;
			attachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			attachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			attachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
			attachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

			configure.invoke(attachmentState);
			});
	}

	VulkanPipelineBuilder& configureDepthStencilState(core::Action<VkPipelineDepthStencilStateCreateInfo&>::Callback configure) {
		configureDepthStencilState_ += configure;

		return *this;
	}

	VulkanPipelineBuilder& withDepthTest(bool enable) {
		return configureDepthStencilState([enable](VkPipelineDepthStencilStateCreateInfo& depthStencilState) {
			depthStencilState.depthTestEnable = enable ? VK_TRUE : VK_FALSE;
		});
	}

	VulkanPipelineBuilder& withDepthWrite(bool enable) {
		return configureDepthStencilState([enable](VkPipelineDepthStencilStateCreateInfo& depthStencilState) {
			depthStencilState.depthWriteEnable = enable ? VK_TRUE : VK_FALSE;
			});
	}

	VulkanPipelineBuilder& withDepthTestAndWrite(bool enable) {
		return configureDepthStencilState([enable](VkPipelineDepthStencilStateCreateInfo& depthStencilState) {
			depthStencilState.depthTestEnable = enable ? VK_TRUE : VK_FALSE;
			depthStencilState.depthWriteEnable = enable ? VK_TRUE : VK_FALSE;
		});
	}

	VulkanPipelineBuilder& addShaderStage(core::Action<VkPipelineShaderStageCreateInfo&>::Callback configure) {
		configureShaderStages_.emplace_back() += configure;

		return *this;
	}

	VulkanPipelineBuilder& withShaderStage(const ShaderRecord& record) {
		addShaderStage([&record](auto& shaderInfo) {
			shaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderInfo.stage = record.type;
			shaderInfo.module = record.module;
			shaderInfo.pName = "main"; // Assuming entry point is "main"
		});

		return *this;
	}

	bool validate() {
		if (!validateFundamentals()) {
			return false;
		}

		if (!validateShaderStages()) {
			return false;
		}

		if (!validateColorBlendState()) {
			return false;
		}
		return true;
	}

	VkPipeline build() {
		if (!validateFundamentals()) {
			throw std::runtime_error("Pipeline validation failed");
		}

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

		bool hasVertex = false, hasFragment = false, hasCompute = false;

		for (auto& configure : configureShaderStages_) {
			VkPipelineShaderStageCreateInfo shaderInfo{};
			configure.invoke(shaderInfo);

			if (shaderInfo.stage == VK_SHADER_STAGE_VERTEX_BIT) {
				hasVertex = true;
			}
			else if (shaderInfo.stage == VK_SHADER_STAGE_FRAGMENT_BIT) {
				hasFragment = true;
			}
			else if (shaderInfo.stage == VK_SHADER_STAGE_COMPUTE_BIT) {
				hasCompute = true;
			}

			shaderStages.push_back(shaderInfo);
		}

		VkGraphicsPipelineCreateInfo pipelineInfo{
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.stageCount = static_cast<uint32_t>(shaderStages.size()),
			// TODO: looks like I neeed to build a linked list of shaders :(
			.pStages = shaderStages.data()
		};

		if (configureColorBlendState_ || !configureColorBlendAttachments_.empty()) {

			VkPipelineColorBlendStateCreateInfo colorBlendState = buildColorBlendState();

			if (!validateColorBlendState(colorBlendState)) {
				throw std::runtime_error("Pipeline validation failed: invalid blend state");
			}

			pipelineInfo.pColorBlendState = &colorBlendState;
		}

		if (configureDepthStencilState_) {
			VkPipelineDepthStencilStateCreateInfo depthStencilState = buildDepthStencilState();

			if (!validateDepthStencilState(depthStencilState)) {
				throw std::runtime_error("Pipeline validation failed: invalid depth stencil state");
			}

			pipelineInfo.pDepthStencilState = &depthStencilState;
		}

		VkPipeline pipeline;
		vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);

		return pipeline;
	}

private:	
	VkPipelineColorBlendStateCreateInfo buildColorBlendState() {
		VkPipelineColorBlendStateCreateInfo colorBlendState{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		};

		if (configureColorBlendState_) {
			configureColorBlendState_.invoke(colorBlendState);
		}

		std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
		for (auto& configureAttachment : configureColorBlendAttachments_) {
			VkPipelineColorBlendAttachmentState attachmentState{};
			configureAttachment.invoke(attachmentState);
			colorBlendAttachments.push_back(attachmentState);
		}

		colorBlendState.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size());
		colorBlendState.pAttachments = colorBlendAttachments.data();

		return colorBlendState;
	}

	VkPipelineDepthStencilStateCreateInfo buildDepthStencilState() {
		VkPipelineDepthStencilStateCreateInfo depthStencilState{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		};

		if (configureDepthStencilState_) {
			configureDepthStencilState_.invoke(depthStencilState);
		}

		return depthStencilState;
	}
	bool validateShaderStages(bool hasVertex, bool hasFragment, bool hasCompute) {
		if (hasCompute && (hasVertex || hasFragment)) {
			EMBER_LOG_ERROR("Cannot mix compute stage with graphics stages");
			return false;
		}
		if (!hasCompute && !hasVertex) {
			EMBER_LOG_ERROR("Graphics pipeline requires at least a vertex stage");
			return false;
		}
		return true;
	}

	bool validateShaderStages() {
		bool hasVertex = false, hasFragment = false, hasCompute = false;
		for (auto& configure : configureShaderStages_) {
			VkPipelineShaderStageCreateInfo shaderInfo{};
			configure.invoke(shaderInfo);
			if (shaderInfo.stage == VK_SHADER_STAGE_VERTEX_BIT) {
				hasVertex = true;
			}
			else if (shaderInfo.stage == VK_SHADER_STAGE_FRAGMENT_BIT) {
				hasFragment = true;
			}
			else if (shaderInfo.stage == VK_SHADER_STAGE_COMPUTE_BIT) {
				hasCompute = true;
			}
		}

		return validateShaderStages(hasVertex, hasFragment, hasCompute);
	}

	bool validateFundamentals() {
		if (device_ == VK_NULL_HANDLE) {
			EMBER_LOG_ERROR("Pipeline builder requires a valid Vulkan device");
			return false;
		}

		if (configureShaderStages_.empty()) {
			EMBER_LOG_ERROR("Pipeline must have at least one shader stage");
			return false;
		}

		return true;
	}

	bool validateColorBlendAttachmentState(const VkPipelineColorBlendAttachmentState& attachmentState) {
		if (attachmentState.blendEnable && attachmentState.colorWriteMask == 0) {
			EMBER_LOG_ERROR("Color blend attachment state has blending enabled but no color write mask set");
			return false;
		}
		return true;
	}

	bool validateColorBlendState(const VkPipelineColorBlendStateCreateInfo& colorBlendState) {
		if (colorBlendState.logicOpEnable && colorBlendState.attachmentCount == 0) {
			EMBER_LOG_ERROR("Color blend state has logicOp enabled but no attachments set");
			return false;
		}

		for (uint32_t i = 0; i < colorBlendState.attachmentCount; ++i) {
			const VkPipelineColorBlendAttachmentState& attachmentState = colorBlendState.pAttachments[i];
			if (!validateColorBlendAttachmentState(attachmentState)) {
				return false;
			}
		}

		return true;
	}

	bool validateColorBlendState() {

		if (configureColorBlendState_ || !configureColorBlendAttachments_.empty()) {
			VkPipelineColorBlendStateCreateInfo colorBlendState = buildColorBlendState();

			if (!validateColorBlendState(colorBlendState)) {
				return false;
			}
		}

		return true;
	}

	bool validateDepthStencilState(const VkPipelineDepthStencilStateCreateInfo& depthStencilState) {
		if (depthStencilState.depthTestEnable && !depthStencilState.depthWriteEnable) {
			EMBER_LOG_WARN("Depth test is enabled but depth write is disabled");
		}

		return true;
	}

	bool validateDepthStencilState() {
		if (configureDepthStencilState_) {
			VkPipelineDepthStencilStateCreateInfo depthStencilState = buildDepthStencilState();
			if (!validateDepthStencilState(depthStencilState)) {
				return false;
			}
		}
		return true;
	}

	VkDevice device_ = VK_NULL_HANDLE;
	core::Action<VkPipelineColorBlendStateCreateInfo&> configureColorBlendState_;
	std::vector<core::Action<VkPipelineColorBlendAttachmentState&>> configureColorBlendAttachments_;
	core::Action<VkPipelineDepthStencilStateCreateInfo&> configureDepthStencilState_;
	std::vector<core::Action<VkPipelineShaderStageCreateInfo&>> configureShaderStages_;
};

}