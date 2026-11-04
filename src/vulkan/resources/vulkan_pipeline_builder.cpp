#include "vulkan_pipeline_builder.h"

#include <logger.h>

namespace ember::vulkan {

VulkanPipelineBuilder& VulkanPipelineBuilder::withDevice(VkDevice device) {
	device_ = device;

	return *this;
}

VulkanPipelineBuilder& VulkanPipelineBuilder::configureColorBlendState(core::Action<VkPipelineColorBlendStateCreateInfo&>::Callback configure) {
	configureColorBlendState_ += configure;

	return *this;
}

VulkanPipelineBuilder& VulkanPipelineBuilder::addBlendAttachmentState(core::Action<VkPipelineColorBlendAttachmentState&>::Callback configure) {
	configureColorBlendAttachments_.emplace_back() += configure;

	return *this;
}

VulkanPipelineBuilder& VulkanPipelineBuilder::addOpaqueBlendState(core::Action<VkPipelineColorBlendAttachmentState&>::OptionalCallback configure) {
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

VulkanPipelineBuilder& VulkanPipelineBuilder::addTransparentBlendState(core::Action<VkPipelineColorBlendAttachmentState&>::OptionalCallback configure) {
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

VulkanPipelineBuilder& VulkanPipelineBuilder::configureDepthStencilState(core::Action<VkPipelineDepthStencilStateCreateInfo&>::Callback configure) {
	configureDepthStencilState_ += configure;

	return *this;
}

VulkanPipelineBuilder& VulkanPipelineBuilder::withDepthTest(bool enable) {
	return configureDepthStencilState([enable](VkPipelineDepthStencilStateCreateInfo& depthStencilState) {
		depthStencilState.depthTestEnable = enable ? VK_TRUE : VK_FALSE;
		});
}

VulkanPipelineBuilder& VulkanPipelineBuilder::withDepthWrite(bool enable) {
	return configureDepthStencilState([enable](VkPipelineDepthStencilStateCreateInfo& depthStencilState) {
		depthStencilState.depthWriteEnable = enable ? VK_TRUE : VK_FALSE;
		});
}

VulkanPipelineBuilder& VulkanPipelineBuilder::withDepthTestAndWrite(bool enable) {
	return configureDepthStencilState([enable](VkPipelineDepthStencilStateCreateInfo& depthStencilState) {
		depthStencilState.depthTestEnable = enable ? VK_TRUE : VK_FALSE;
		depthStencilState.depthWriteEnable = enable ? VK_TRUE : VK_FALSE;
		});
}

VulkanPipelineBuilder& VulkanPipelineBuilder::addShaderStage(core::Action<VkPipelineShaderStageCreateInfo&>::Callback configure) {
	configureShaderStages_.emplace_back() += configure;

	return *this;
}

VulkanPipelineBuilder& VulkanPipelineBuilder::withShaderStage(const ShaderRecord& record) {
	addShaderStage([&record](auto& shaderInfo) {
		shaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderInfo.stage = record.type;
		shaderInfo.module = record.module;
		shaderInfo.pName = "main"; // Assuming entry point is "main"
		});

	return *this;
}

VkPipeline VulkanPipelineBuilder::build() {
	if (!validateFundamentals()) {
		throw std::runtime_error("Pipeline validation failed");
	}

	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
	shaderStages.reserve(configureShaderStages_.size());

	bool hasVertex = false, hasFragment = false, hasCompute = false;
	for (auto& configureShaderStage : configureShaderStages_) {
		auto& createInfo = shaderStages.emplace_back();
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

		configureShaderStage.invoke(createInfo);
	}

	validateShaderStages(shaderStages);

	auto pDepthStencilState = std::make_unique<VkPipelineDepthStencilStateCreateInfo>();
	pDepthStencilState->sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	if (configureDepthStencilState_) {
		auto& createInfo = *pDepthStencilState;
		configureDepthStencilState_.invoke(createInfo);

		if (!validateDepthStencilState(createInfo)) {
			throw std::runtime_error("Pipeline validation failed: invalid depth stencil state");
		}
	}

	auto pColorBlendState = std::make_unique<VkPipelineColorBlendStateCreateInfo>();
	pColorBlendState->sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	if (configureColorBlendState_) {
		auto& createInfo = *pColorBlendState;
		configureColorBlendState_.invoke(createInfo);

		if (!validateColorBlendState(*pColorBlendState)) {
			throw std::runtime_error("Pipeline validation failed: invalid blend state");
		}
	}

	std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates;
	colorBlendAttachmentStates.reserve(configureColorBlendAttachments_.size());

	if (configureColorBlendAttachments_.size() > 0) {
		for (auto& configureAttachment : configureColorBlendAttachments_) {
			auto& attachmentState = colorBlendAttachmentStates.emplace_back();
			configureAttachment.invoke(attachmentState);
		}

		if (!validateColorBlendAttachmentStates(colorBlendAttachmentStates)) {
			throw std::runtime_error("Pipeline validation failed: invalid blend state");
		}

		pColorBlendState->attachmentCount = static_cast<uint32_t>(colorBlendAttachmentStates.size());
		pColorBlendState->pAttachments = colorBlendAttachmentStates.data();
	}

	VkGraphicsPipelineCreateInfo pipelineInfo{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount = static_cast<uint32_t>(shaderStages.size()),
		.pStages = shaderStages.data(),
		.pDepthStencilState = pDepthStencilState.get(),
		.pColorBlendState = pColorBlendState.get()
	};

	VkPipeline pipeline;
	vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);

	return pipeline;
}

bool VulkanPipelineBuilder::validateShaderStages(const std::span<VkPipelineShaderStageCreateInfo>& shaderStages) {
	bool hasVertex = false, hasFragment = false, hasCompute = false;
	for (const auto& createInfo : shaderStages) {
		if (createInfo.stage == VK_SHADER_STAGE_VERTEX_BIT) {
			hasVertex = true;
		}
		else if (createInfo.stage == VK_SHADER_STAGE_FRAGMENT_BIT) {
			hasFragment = true;
		}
		else if (createInfo.stage == VK_SHADER_STAGE_COMPUTE_BIT) {
			hasCompute = true;
		}
	}

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

bool VulkanPipelineBuilder::validateFundamentals() {
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

bool VulkanPipelineBuilder::validateColorBlendAttachmentState(const VkPipelineColorBlendAttachmentState& attachmentState) {
	if (attachmentState.blendEnable && attachmentState.colorWriteMask == 0) {
		EMBER_LOG_ERROR("Color blend attachment state has blending enabled but no color write mask set");
		return false;
	}
	return true;
}

bool VulkanPipelineBuilder::validateColorBlendState(const VkPipelineColorBlendStateCreateInfo& colorBlendState) {
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

bool VulkanPipelineBuilder::validateColorBlendAttachmentStates(const std::span<VkPipelineColorBlendAttachmentState>& attachmentStates) {
	if (attachmentStates.empty()) {
		EMBER_LOG_ERROR("Color blend state has logicOp enabled but no attachments set");
		return false;
	}

	for (const auto& attachmentState : attachmentStates) {
		if (!validateColorBlendAttachmentState(attachmentState)) {
			return false;
		}
	}

	return true;
}

bool VulkanPipelineBuilder::validateDepthStencilState(const VkPipelineDepthStencilStateCreateInfo& depthStencilState) {
	if (depthStencilState.depthTestEnable && !depthStencilState.depthWriteEnable) {
		EMBER_LOG_WARN("Depth test is enabled but depth write is disabled");
	}

	return true;
}

}