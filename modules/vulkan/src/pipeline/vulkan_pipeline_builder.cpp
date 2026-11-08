#include <vulkan/pipeline/vulkan_pipeline_builder.hpp>

#include <core/logger.hpp>

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

VkPipeline VulkanPipelineBuilder::build() const {
	if (!validateFundamentals()) {
		throw std::runtime_error("Pipeline validation failed");
	}

	auto shaderStages = buildShaderStages();
	if (!validateShaderStages(shaderStages)) {
		throw std::runtime_error("Pipeline validation failed: invalid shader stages");
	}

	auto pVertexInputState = buildVertexInputState();
	if (!validateVertexInputState(*pVertexInputState)) {
		throw std::runtime_error("Pipeline validation failed: invalid vertex input state");
	}

	auto pInputAssemblyState = buildInputAssemblyState();
	if (!validateInputAssemblyState(*pInputAssemblyState)) {
		throw std::runtime_error("Pipeline validation failed: invalid input assembly state");
	}

	auto pViewportState = buildViewportState();
	if (!validateViewportState(*pViewportState)) {
		throw std::runtime_error("Pipeline validation failed: invalid viewport state");
	}

	auto pRasterizationState = buildRasterizationState();

	auto pDepthStencilState = buildDepthStencilState();
	if (!validateDepthStencilState(*pDepthStencilState)) {
		throw std::runtime_error("Pipeline validation failed: invalid depth stencil state");
	}

	auto colorBlendAttachmentStates = buildColorBlendAttachmentStates();
	auto pColorBlendState = buildColorBlendState(colorBlendAttachmentStates);

	if (!validateColorBlendState(*pColorBlendState)) {
		throw std::runtime_error("Pipeline validation failed: invalid color blend state");
	}

	auto pDynamicState = buildDynamicState();
	if (!validateDynamicState(*pDynamicState)) {
		throw std::runtime_error("Pipeline validation failed: invalid dynamic state");
	}

	VkGraphicsPipelineCreateInfo pipelineInfo{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount = static_cast<uint32_t>(shaderStages.size()),
		.pStages = shaderStages.data(),
		.pVertexInputState = pVertexInputState.get(),
		.pInputAssemblyState = pInputAssemblyState.get(),
		.pViewportState = pViewportState.get(),
		.pRasterizationState = pRasterizationState.get(),
		.pDepthStencilState = pDepthStencilState.get(),
		.pColorBlendState = pColorBlendState.get(),
		.pDynamicState = pDynamicState.get(),
		
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

bool VulkanPipelineBuilder::validateFundamentals() const {
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

bool VulkanPipelineBuilder::validateVertexInputState(const VkPipelineVertexInputStateCreateInfo& vertexInputState) {
	return true;
}

bool VulkanPipelineBuilder::validateInputAssemblyState(const VkPipelineInputAssemblyStateCreateInfo& inputAssemblyState) {
	return true;
}

bool VulkanPipelineBuilder::validateViewportState(const VkPipelineViewportStateCreateInfo& viewportState) {
	return true;
}

bool VulkanPipelineBuilder::validateRasterizationState(const VkPipelineRasterizationStateCreateInfo& rasterizationState) {
	return true;
}

bool VulkanPipelineBuilder::validateMultisampleState(const VkPipelineMultisampleStateCreateInfo& multisampleState) {
	return true;
}

bool VulkanPipelineBuilder::validateDepthStencilState(const VkPipelineDepthStencilStateCreateInfo& depthStencilState) {
	if (depthStencilState.depthTestEnable && !depthStencilState.depthWriteEnable) {
		EMBER_LOG_WARN("Depth test is enabled but depth write is disabled");
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

bool VulkanPipelineBuilder::validateDynamicState(const VkPipelineDynamicStateCreateInfo& dynamicState) {
	return true;
}

std::vector<VkPipelineShaderStageCreateInfo> VulkanPipelineBuilder::buildShaderStages() const {
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

	shaderStages.reserve(configureShaderStages_.size());
	for (auto& configureShaderStage : configureShaderStages_) {
		auto& createInfo = shaderStages.emplace_back();
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

		configureShaderStage.invoke(createInfo);
	}

	return shaderStages;
}

std::unique_ptr<VkPipelineDynamicStateCreateInfo> VulkanPipelineBuilder::buildDynamicState() const {
	auto pDynamicState = std::make_unique<VkPipelineDynamicStateCreateInfo>();
	pDynamicState->sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

	return pDynamicState;
}

std::unique_ptr<VkPipelineVertexInputStateCreateInfo> VulkanPipelineBuilder::buildVertexInputState() const {
	auto pVertexInputState = std::make_unique<VkPipelineVertexInputStateCreateInfo>();
	pVertexInputState->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	return pVertexInputState;
}

std::unique_ptr<VkPipelineViewportStateCreateInfo> VulkanPipelineBuilder::buildViewportState() const {
	auto pViewportState = std::make_unique<VkPipelineViewportStateCreateInfo>();
	pViewportState->sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

	return pViewportState;
}

std::unique_ptr<VkPipelineRasterizationStateCreateInfo> VulkanPipelineBuilder::buildRasterizationState() const {
	auto pRasterizationState = std::make_unique<VkPipelineRasterizationStateCreateInfo>();
	pRasterizationState->sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

	return pRasterizationState;
}

std::unique_ptr<VkPipelineMultisampleStateCreateInfo> VulkanPipelineBuilder::buildMultisampleState() const {
	auto pMultisampleState = std::make_unique<VkPipelineMultisampleStateCreateInfo>();
	pMultisampleState->sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	return pMultisampleState;
}

std::unique_ptr<VkPipelineInputAssemblyStateCreateInfo> VulkanPipelineBuilder::buildInputAssemblyState() const {
	auto pInputAssemblyState = std::make_unique<VkPipelineInputAssemblyStateCreateInfo>();
	pInputAssemblyState->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

	return pInputAssemblyState;
}

std::unique_ptr<VkPipelineDepthStencilStateCreateInfo> VulkanPipelineBuilder::buildDepthStencilState() const {
	auto pDepthStencilState = std::make_unique<VkPipelineDepthStencilStateCreateInfo>();
	pDepthStencilState->sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

	if (configureDepthStencilState_) {
		auto& createInfo = *pDepthStencilState;
		configureDepthStencilState_.invoke(createInfo);
	}

	return pDepthStencilState;
}

std::vector<VkPipelineColorBlendAttachmentState> VulkanPipelineBuilder::buildColorBlendAttachmentStates() const {
	std::vector<VkPipelineColorBlendAttachmentState> attachmentStates;

	attachmentStates.reserve(configureColorBlendAttachments_.size());
	for (auto& configureAttachmentState : configureColorBlendAttachments_) {
		auto& attachmentState = attachmentStates.emplace_back();
		configureAttachmentState.invoke(attachmentState);
	}

	return attachmentStates;
}

std::unique_ptr<VkPipelineColorBlendStateCreateInfo> ember::vulkan::VulkanPipelineBuilder::buildColorBlendState(const std::span<VkPipelineColorBlendAttachmentState> attachmentStates) const {
	auto pColorBlendState = std::make_unique<VkPipelineColorBlendStateCreateInfo>();
	pColorBlendState->sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

	if (configureColorBlendState_) {
		auto& createInfo = *pColorBlendState;
		configureColorBlendState_.invoke(createInfo);
	}

	pColorBlendState->attachmentCount = static_cast<uint32_t>(attachmentStates.size());	
	pColorBlendState->pAttachments = attachmentStates.data();

	return pColorBlendState;
}

} // namespace ember::vulkan
