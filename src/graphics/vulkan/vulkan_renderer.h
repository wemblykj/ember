#pragma once

#include "../renderer.h"
#include "vulkan_context.h"
#include <memory>
#include <unordered_map>

namespace ember::graphics::vulkan {

/**
 * Vulkan renderer implementation
 */
class VulkanRenderer : public Renderer {
public:
    explicit VulkanRenderer(const RendererConfig& config);
    ~VulkanRenderer() override;

    bool initialize() override;
    void shutdown() override;

    ResourceGroupID createResourceGroup(const ResourceGroupDesc& desc) override;
    void releaseResourceGroup(ResourceGroupID group) override;

    TechniqueID registerTechnique(const TechniqueDesc& desc) override;
    MaterialID registerMaterial(const MaterialDesc& desc) override;
    GeometryID registerGeometry(const GeometryDesc& desc) override;
    void unregisterGeometry(GeometryID id) override;

    void beginFrame() override;
    void endFrame() override;
    void present() override;
    void resizeFramebuffer(uint32_t width, uint32_t height) override;

private:
    struct ResourceGroupMembers {
        std::vector<TechniqueID> techniques;
        std::vector<MaterialID> materials;
        std::vector<GeometryID> geometries;
	};

    RendererConfig config_;
    std::unique_ptr<VulkanContext> context_;
	ResourceGroupID nextResourceGroupId_ = BuiltinResourceGroup::Custom;
    TechniqueID nextTechniqueId_ = BuiltinTechnique::Custom;
	MaterialID nextMaterialId_ = BuiltinMaterial::Custom;
    GeometryID nextGeometryId_ = BuiltinGeometry::Custom;
    std::unordered_map<ResourceGroupID, ResourceGroupMembers> groupMembers_;
};

/**
 * Factory function for creating Vulkan renderers
 */
RendererPtr createRenderer(const RendererConfig& config, platform::Window* window);

}  // namespace ember::graphics::vulkan
