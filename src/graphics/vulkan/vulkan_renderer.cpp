#include "vulkan_renderer.h"
#include "../../core/logger.h"

namespace ember::graphics::vulkan {

VulkanRenderer::VulkanRenderer(const RendererConfig& config)
    : config_(config) {
}

VulkanRenderer::~VulkanRenderer() {
    shutdown();
}

bool VulkanRenderer::initialize() {
    using namespace ember::core;

    EMBER_LOG_INFO("Initializing Vulkan renderer...");

    context_ = std::make_unique<VulkanContext>(config_);

    EMBER_LOG_INFO("Vulkan renderer initialized");
    return true;
}

void VulkanRenderer::shutdown() {
    using namespace ember::core;

    if (context_) {
        context_->shutdown();
        context_.reset();
    }

    EMBER_LOG_INFO("Vulkan renderer shutdown");
}

ResourceGroupID VulkanRenderer::createResourceGroup(const ResourceGroupDesc& desc) {
    ResourceGroupID id = nextGroupId_++;
    //groupNames_[id] = desc.debugName;
    return id;
}

void VulkanRenderer::releaseResourceGroup(ResourceGroupID group) {
    auto it = groupMembers_.find(group);
    if (it == groupMembers_.end()) return;

    //for (auto id : it->second.materials) { destroyMaterial(id);  materials_.erase(id); }
    //for (auto id : it->second.techniques) { destroyTechnique(id); techniques_.erase(id); }
    //for (auto id : it->second.geometry) { destroyGeometry(id);  geometryBuffers_.erase(id); }

    groupMembers_.erase(it);
    EMBER_LOG_INFO("Released resource group '{}'", groupNames_[group]);
    groupNames_.erase(group);
}

TechniqueID VulkanRenderer::registerTechnique(const TechniqueDesc& desc, ResourceGroupID group) {
    TechniqueID id = nextTechniqueId_++;
    //techniquePipelines_[id] = compilePipeline(desc); // expensive, done once
    groupMembers_[group].techniques.push_back(id);
    return id;
}

MaterialID VulkanRenderer::registerMaterial(const MaterialDesc& desc, ResourceGroupID group) {
    MaterialID id = nextMaterialId_++;
    //materials_[id] = compileMaterial(desc); // expensive, done once
    groupMembers_[group].materials.push_back(id);
    return id;
}

GeometryID VulkanRenderer::registerGeometry(const GeometryDesc& desc, ResourceGroupID group) {
    GeometryID id = nextGeometryId_++;
    //geometryPipelines_[id] = compilePipeline(desc); // expensive, done once
    groupMembers_[group].geometries.push_back(id);
    return id;
}

void VulkanRenderer::beginFrame() {
    // Implement frame begin logic
}

void VulkanRenderer::submitPass(const RendererPass& pass) {
    for (const auto& bucket : pass.buckets) {
        VkPipeline pipeline = lookupPipelineForMaterial(bucket.materialId); // cheap lookup
        vkCmdBindPipeline(cmdBuf_, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        for (const auto& packet : bucket.packets) {
            // bind per-draw data, vkCmdDrawIndexed, etc.
        }
    }
}

void VulkanRenderer::endFrame() {
    // Implement frame end logic
}

void VulkanRenderer::present() {
    // Implement present logic
}

void VulkanRenderer::resizeFramebuffer(uint32_t width, uint32_t height) {
    config_.width = width;
    config_.height = height;
}

RendererPtr createRenderer(const RendererConfig& config, platform::Window* window) {
    auto renderer = std::make_unique<VulkanRenderer>(config);
    if (renderer->initialize()) {
        return renderer;
    }
    return nullptr;
}

}  // namespace ember::graphics::vulkan
