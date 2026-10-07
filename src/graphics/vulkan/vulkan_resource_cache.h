#pragma once

#include <cstdint>
#include <vector>

#include <vulkan/vulkan.h>

#include <resource_cache.h>

#include "../resources.h" // ResourceGroupID, TechniqueID, MaterialID, GeometryID, Builtin*
#include "vulkan_context.h"
#include "vulkan_resource_compiler.h"

namespace ember::graphics::vulkan {

/**
    * @brief Vulkan resource cache implementation
    */
class VulkanResourceCache : public ResourceCache {
public:
    VulkanResourceCache(const ResourceCacheConfig& config, 
    	std::shared_ptr<VulkanContext> context,
        std::unique_ptr<VulkanResourceCompiler> compiler);
    ~VulkanResourceCache() override;

    // VulkanResourceCache
public:
    TechniqueRecord ResolveTechnique(TechniqueID id);
    MaterialRecord ResolveMaterial(MaterialID id);
    GeometryRecord ResolveGeometry(GeometryID id);

    // ResourceCache
public:
    VertexFormatID registerVertexFormat(const VertexFormatDesc& desc) override;
    void unregisterVertexFormat(VertexFormatID id) override;

    void clearAllResources() override;
    ResourceGroupID createResourceGroup(const ResourceGroupDesc& desc) override;
    void releaseResourceGroup(ResourceGroupID group) override;

    bool assertTechnique(TechniqueID id) override;
    bool assertMaterial(MaterialID id) override;
    bool assertGeometry(GeometryID id) override;

    TechniqueID registerTechnique(const TechniqueDesc& desc, ResourceGroupID group) override;
    MaterialID registerMaterial(const MaterialDesc& desc, ResourceGroupID group) override;
    GeometryID registerGeometry(const GeometryDesc& desc, ResourceGroupID group) override;

    TechniqueID setInvalidTechniqueId(TechniqueID id) override;
    TechniqueID setUnassignedTechniqueId(TechniqueID id) override;
    TechniqueID setUnresolvedTechniqueId(TechniqueID id) override;

    MaterialID setInvalidMaterialId(MaterialID id) override;
    MaterialID setUnassignedMaterialId(MaterialID id) override;
    MaterialID setUnresolvedMaterialId(MaterialID id) override;

    GeometryID setInvalidGeometryId(GeometryID id) override;

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
        ResourceGroupID group = BuiltinResourceGroup::Default;  ///< Owning resource group
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
        GeometryAllocation allocation;           ///< Memory backing vertex and index buffers
        bool uploaded = false;                   ///< True if resident on device
    };

    ResourceCacheConfig config_;

    std::shared_ptr<VulkanContext> context_;
    std::unique_ptr<VulkanResourceCompiler> compiler_;

	TechniqueID invalidTechniqueId_ = BuiltinTechnique::Undefined;
    TechniqueID unassignedTechniqueId_ = BuiltinTechnique::Undefined;
	TechniqueID unresolvedTechniqueId_ = BuiltinTechnique::Undefined;

	MaterialID invalidMaterialId_ = BuiltinMaterial::Undefined;
    MaterialID unassignedMaterialId_ = BuiltinMaterial::Undefined;
    MaterialID unresolvedMaterialId_ = BuiltinMaterial::Undefined;

	GeometryID invalidGeometryId_ = BuiltinGeometry::Undefined;

    VertexFormatID nextVertexFormatId_ = BuiltinVertexFormat::Custom;
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

} // namespace ember::graphics::vulkan