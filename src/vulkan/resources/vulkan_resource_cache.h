#pragma once

#include <cstdint>
#include <vector>

#include <vulkan/vulkan.h>

#include <resource_types.h>
#include <resource_cache.h>

#include "../context/vulkan_context.h"
#include "vulkan_resource_builder.h"

using TechniqueID = ember::resources::TechniqueID;
using MaterialID = ember::resources::MaterialID;
using GeometryID = ember::resources::GeometryID;
using ResourceGroupID = ember::resources::ResourceGroupID;
using VertexFormatID = ember::resources::VertexFormatID;

using TechniqueDesc = ember::resources::TechniqueDesc;
using MaterialDesc = ember::resources::MaterialDesc;
using GeometryDesc = ember::resources::GeometryDesc;
using ResourceGroupDesc = ember::resources::ResourceGroupDesc;

namespace ember::vulkan {

/**
    * @brief Vulkan resource cache implementation
    */
class VulkanResourceCache : public resources::ResourceCache {
public:
    VulkanResourceCache(const resources::ResourceCacheConfig& config, 
    	std::shared_ptr<VulkanContext> context,
        std::unique_ptr<VulkanResourceBuilder> compiler);
    ~VulkanResourceCache() override;

    // VulkanResourceCache
public:
    const TechniqueRecord& ResolveTechnique(TechniqueID id);
    const MaterialRecord& ResolveMaterial(MaterialID id);
    const GeometryRecord& ResolveGeometry(GeometryID id);
	const VertexFormatRecord& ResolveVertexFormat(VertexFormatID id);

    // ResourceCache
public:
    void clearAllResources() override;
    ResourceGroupID createResourceGroup(const ResourceGroupDesc& desc) override;
    void destroyVertexFormat(std::vector<unsigned>::value_type id);
    void releaseResourceGroup(ResourceGroupID group) override;
    bool assertTechnique(TechniqueID id) override;
    bool assertMaterial(MaterialID id) override;
    bool assertGeometry(GeometryID id) override;
    bool assertVertexFormat(VertexFormatID id) override;

    TechniqueID registerTechnique(const TechniqueDesc& desc, ResourceGroupID group) override;
    MaterialID registerMaterial(const MaterialDesc& desc, ResourceGroupID group) override;
    GeometryID registerGeometry(const GeometryDesc& desc, ResourceGroupID group) override;
    VertexFormatID registerVertexFormat(const VertexFormatDesc& desc, ResourceGroupID group) override;

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

    void addTechnique(ResourceGroupID group, TechniqueID id, const TechniqueRecord& record);
    void addMaterial(ResourceGroupID group, MaterialID id, const MaterialRecord& record);
    void addGeometry(ResourceGroupID group, GeometryID id, const GeometryRecord& record, const GeometryAllocation& allocation);
    void addVertexFormat(ResourceGroupID group, VertexFormatID id, const VertexFormatRecord& record);

private:
    struct ResourceGroupMembers {
        std::vector<TechniqueID> techniques;
        std::vector<MaterialID> materials;
        std::vector<GeometryID> geometry;
        std::vector<VertexFormatID> vertexFormats;
    };

    struct ResourceCacheEntry {
        ResourceGroupID group = resources::BuiltinResourceGroup::Global;  ///< Owning resource group
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

    struct VertexFormatEntry : ResourceCacheEntry {
        VertexFormatRecord record;
	};

    resources::ResourceCacheConfig config_;

    std::shared_ptr<VulkanContext> context_;
    std::unique_ptr<VulkanResourceBuilder> resourceBuilder_;

	TechniqueID invalidTechniqueId_ = resources::BuiltinTechnique::Undefined;
    TechniqueID unassignedTechniqueId_ = resources::BuiltinTechnique::Undefined;
	TechniqueID unresolvedTechniqueId_ = resources::BuiltinTechnique::Undefined;

	MaterialID invalidMaterialId_ = resources::BuiltinMaterial::Undefined;
    MaterialID unassignedMaterialId_ = resources::BuiltinMaterial::Undefined;
    MaterialID unresolvedMaterialId_ = resources::BuiltinMaterial::Undefined;
	GeometryID invalidGeometryId_ = resources::BuiltinGeometry::Undefined;

    ResourceGroupID nextGroupId_ = resources::BuiltinResourceGroup::Custom;
    TechniqueID nextTechniqueId_ = resources::BuiltinTechnique::Custom;
    MaterialID nextMaterialId_ = resources::BuiltinMaterial::Custom;
    GeometryID nextGeometryId_ = resources::BuiltinGeometry::Custom;
    VertexFormatID nextVertexFormatId_ = resources::BuiltinVertexFormat::Custom;

    std::unordered_map<TechniqueID, TechniqueEntry> techniques_;
    std::unordered_map<MaterialID, MaterialEntry> materials_;
    std::unordered_map<GeometryID, GeometryEntry> geometry_;
	std::unordered_map<VertexFormatID, VertexFormatEntry> vertexFormats_;

    std::unordered_map<ResourceGroupID, ResourceGroupMembers> groupMembers_;
    std::unordered_map<ResourceGroupID, std::string> groupNames_;

};

} // namespace ember::vulkan