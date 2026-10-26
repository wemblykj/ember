#pragma once

#include <cstdint>
#include <vector>

#include <vulkan/vulkan.h>

#include <resource_types.h>
#include <resource_cache.h>
#include <canned_resource_provider.h>

#include "../context/vulkan_context.h"

#include "vulkan_resource_builder.h"
#include "vulkan_resource_resolver.h"

using ShaderID = ember::resources::ShaderID;
using TechniqueID = ember::resources::TechniqueID;
using MaterialID = ember::resources::MaterialID;
using GeometryID = ember::resources::GeometryID;
using ResourceGroupID = ember::resources::ResourceGroupID;
using VertexFormatID = ember::resources::VertexFormatID;

using ShaderDesc = ember::resources::ShaderDesc;
using TechniqueDesc = ember::resources::TechniqueDesc;
using MaterialDesc = ember::resources::MaterialDesc;
using GeometryDesc = ember::resources::GeometryDesc;
using ResourceGroupDesc = ember::resources::ResourceGroupDesc;

namespace ember::vulkan {

/**
    * @brief Vulkan resource cache implementation
    */
class VulkanResourceCache : virtual public resources::ResourceCache, virtual public VulkanResourceResolver {
public:
    VulkanResourceCache(const resources::ResourceCacheConfig& config, 
    	std::shared_ptr<VulkanContext> context,
        std::unique_ptr<VulkanResourceBuilder> resourceBuilder,
        std::unique_ptr<resources::CannedResourceProvider> cannedResourceProvider);
    ~VulkanResourceCache() override;

    // VulkanResourceResolver
public:
	const ShaderRecord& resolveShader(ShaderID id) const override;
    const TechniqueRecord& resolveTechnique(TechniqueID id) const override;
    const MaterialRecord& resolveMaterial(MaterialID id) const override;
    const GeometryRecord& resolveGeometry(GeometryID id) const override;
	const VertexFormatRecord& resolveVertexFormat(VertexFormatID id) const override;

    // ResourceCache
public:
    void clearAllResources() override;
    ResourceGroupID createResourceGroup(const ResourceGroupDesc& desc) override;
    void releaseResourceGroup(ResourceGroupID group) override;
    bool assertShader(ShaderID id) override;
    bool assertTechnique(TechniqueID id) override;
    bool assertMaterial(MaterialID id) override;
    bool assertGeometry(GeometryID id) override;
    bool assertVertexFormat(VertexFormatID id) override;

	ShaderID registerShader(const ShaderDesc& desc, ResourceGroupID group) override;
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
    void destroyShader(ShaderID id);
    void destroyMaterial(MaterialID id);
    void destroyTechnique(TechniqueID id);
    void destroyGeometry(GeometryID id);
    void destroyVertexFormat(VertexFormatID id);

    void addShader(ResourceGroupID group, ShaderID id, const ShaderRecord& record);
    void addTechnique(ResourceGroupID group, TechniqueID id, const TechniqueRecord& record);
    void addMaterial(ResourceGroupID group, MaterialID id, const MaterialRecord& record);
    void addGeometry(ResourceGroupID group, GeometryID id, const GeometryRecord& record, const GeometryAllocation& allocation);
    void addVertexFormat(ResourceGroupID group, VertexFormatID id, const VertexFormatRecord& record);

private:
    struct ResourceGroupMembers {
		std::vector<ShaderID> shaders;
        std::vector<TechniqueID> techniques;
        std::vector<MaterialID> materials;
        std::vector<GeometryID> geometry;
        std::vector<VertexFormatID> vertexFormats;
    };

    struct ResourceCacheEntry {
        ResourceGroupID group = resources::CannedResourceGroup::Global;  ///< Owning resource group
        std::uint64_t hash = 0;                                 ///< Stable hash of the source desc (0 = not set)
        std::uint32_t refCount = 1;                             ///< Simple reference count
        std::uint64_t lastUsedFrame = 0;                        ///< Last frame index this resource was used on
        std::size_t memorySize = 0;                             ///< Bytes allocated on GPU for this resource
    };

    struct ShaderEntry : ResourceCacheEntry {
        ShaderRecord record;
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
	std::unique_ptr<resources::CannedResourceProvider> cannedResourceProvider_;

	TechniqueID invalidTechniqueId_ = resources::CannedTechnique::Undefined;
    TechniqueID unassignedTechniqueId_ = resources::CannedTechnique::Undefined;
	TechniqueID unresolvedTechniqueId_ = resources::CannedTechnique::Undefined;

	MaterialID invalidMaterialId_ = resources::CannedMaterial::Undefined;
    MaterialID unassignedMaterialId_ = resources::CannedMaterial::Undefined;
    MaterialID unresolvedMaterialId_ = resources::CannedMaterial::Undefined;
	GeometryID invalidGeometryId_ = resources::CannedGeometry::Undefined;

    ResourceGroupID nextGroupId_ = resources::CannedResourceGroup::Custom;
	ShaderID nextShaderId_ = resources::CannedShader::Custom;
    TechniqueID nextTechniqueId_ = resources::CannedTechnique::Custom;
    MaterialID nextMaterialId_ = resources::CannedMaterial::Custom;
    GeometryID nextGeometryId_ = resources::CannedGeometry::Custom;
    VertexFormatID nextVertexFormatId_ = resources::CannedVertexFormat::Custom;

    mutable std::unordered_map<ShaderID, ShaderEntry> shaders_;
    mutable std::unordered_map<TechniqueID, TechniqueEntry> techniques_;
    mutable std::unordered_map<MaterialID, MaterialEntry> materials_;
    mutable std::unordered_map<GeometryID, GeometryEntry> geometry_;
	mutable std::unordered_map<VertexFormatID, VertexFormatEntry> vertexFormats_;

    std::unordered_map<ResourceGroupID, ResourceGroupMembers> groupMembers_;
    std::unordered_map<ResourceGroupID, std::string> groupNames_;

};

} // namespace ember::vulkan