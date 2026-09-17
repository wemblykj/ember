#pragma once

#include "../renderer.h"

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "vulkan_context.h"

namespace ember::graphics::vulkan {

/**
 * Vulkan renderer implementation
 */
class VulkanRenderer : public Renderer {
public:
    explicit VulkanRenderer(const RendererConfig& config);
    ~VulkanRenderer() override;

    bool initialize(platform::SurfaceProvider* provider) override;
    void shutdown() override;

    void clearAllResources() override;
    ResourceGroupID createResourceGroup(const ResourceGroupDesc& desc) override;
    void releaseResourceGroup(ResourceGroupID group) override;

    TechniqueID registerTechnique(const TechniqueDesc& desc, ResourceGroupID group) override;
    MaterialID registerMaterial(const MaterialDesc& desc, ResourceGroupID group) override;
    GeometryID registerGeometry(const GeometryDesc& desc, ResourceGroupID group) override;
    void unregisterTechnique(TechniqueID id) override;
    void unregisterMaterial(MaterialID id) override;
    void unregisterGeometry(GeometryID id) override;

    void beginFrame() override;
    void submitPass(const RendererPass& pass) override;
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
    ResourceGroupID nextGroupId_ = BuiltinResourceGroup::Custom;
    TechniqueID nextTechniqueId_ = BuiltinTechnique::Custom;
    MaterialID nextMaterialId_ = BuiltinMaterial::Custom;
    GeometryID nextGeometryId_ = BuiltinGeometry::Custom;
    std::unordered_map<ResourceGroupID, ResourceGroupMembers> groupMembers_;
    std::unordered_map<ResourceGroupID, std::string> groupNames_;
};

/**
 * Factory function for creating Vulkan renderers
 */
RendererPtr createRenderer(const RendererConfig& config, platform::SurfaceProvider* provider);

}  // namespace ember::graphics::vulkan
