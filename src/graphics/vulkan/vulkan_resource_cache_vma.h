#pragma once

#include "../renderer.h"

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include <vk_mem_alloc.h>

#include "vulkan_context_vma.h"
#include "resource_records.h"

namespace ember::graphics::vulkan {

/**
 * @brief Vulkan resource cache implementation
 */
class VulkanResourceCacheVma : public ResourceCache {
public:
    VulkanResourceCacheVma(const ResourceCacheConfig& config, std::shared_ptr<VulkanContextVma> context);
    ~VulkanResourceCacheVma() override;

    // VulkanResourceCache
public:
    TechniqueRecord ResolveTechnique(TechniqueID id);
    MaterialRecord ResolveMaterial(MaterialID id);
    GeometryRecord ResolveGeometry(GeometryID id);

    // ResourceCache
public:
    void clearAllResources() override;
    ResourceGroupID createResourceGroup(const ResourceGroupDesc& desc) override;
    void releaseResourceGroup(ResourceGroupID group) override;

    bool assertTechnique(TechniqueID id) override;
    bool assertMaterial(MaterialID id) override;
    bool assertGeometry(GeometryID id) override;

    TechniqueID registerTechnique(const TechniqueDesc& desc, ResourceGroupID group) override;
    MaterialID registerMaterial(const MaterialDesc& desc, ResourceGroupID group) override;
    GeometryID registerGeometry(const GeometryDesc& desc, ResourceGroupID group) override;

    MaterialID setUnassignedMaterial(MaterialID id) override;
    MaterialID setUnresolvedMaterial(MaterialID id) override;

private:
    MaterialID bindMaterial(MaterialID id);
    void destroyMaterial(MaterialID id);
    void destroyTechnique(TechniqueID id);
    void destroyGeometry(GeometryID id);

private:
    struct ResourceGroupMembers {
        std::vector<TechniqueID> techniques;
        std::vector<MaterialID> materials;
        std::vector<GeometryID> geometry;
    };

    struct ResourceCacheEntry {
        ResourceGroupID group = BuiltinResourceGroup::Default;   ///< Owning resource group
        std::uint64_t hash = 0;                                 ///< Stable hash of the source desc (0 = not set)
        std::uint32_t refCount = 1;                             ///< Simple reference count
        std::uint64_t lastUsedFrame = 0;                        ///< Last frame index this resource was used on
        std::size_t memorySize = 0;                             ///< Bytes allocated on GPU for this resource
    };

    struct TechniqueEntry : ResourceCacheEntry {
        TechniqueRecord record;
    };

    struct MaterialEntry : ResourceCacheEntry {
        MaterialRecord record;
    };

    struct GeometryEntry : ResourceCacheEntry {
        GeometryRecord record;
        VmaAllocation vertexMemory = VK_NULL_HANDLE;           ///< Memory backing vertex buffer
        VmaAllocation indexMemory = VK_NULL_HANDLE;            ///< Memory backing index buffer
        bool uploaded = false;                                  ///< True if resident on device
    };

    ResourceCacheConfig config_;

    std::shared_ptr<VulkanContextVma> context_;

    MaterialID unassignedMaterial_ = BuiltinMaterial::Undefined;
    MaterialID unresolvedMaterial_ = BuiltinMaterial::Undefined;

    ResourceGroupID nextGroupId_ = BuiltinResourceGroup::Custom;
    TechniqueID nextTechniqueId_ = BuiltinTechnique::Custom;
    MaterialID nextMaterialId_ = BuiltinMaterial::Custom;
    GeometryID nextGeometryId_ = BuiltinGeometry::Custom;

    std::unordered_map<TechniqueID, TechniqueEntry> techniques_;
    std::unordered_map<MaterialID, MaterialEntry> materials_;
    std::unordered_map<GeometryID, GeometryEntry> geometry_;

    std::unordered_map<ResourceGroupID, ResourceGroupMembers> groupMembers_;
    std::unordered_map<ResourceGroupID, std::string> groupNames_;

};

}  // namespace ember::graphics::vulkan
