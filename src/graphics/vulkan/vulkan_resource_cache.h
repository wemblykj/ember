#pragma once

#pragma once

#include <cstdint>
#include <vector>

#include <vulkan/vulkan.h>

#include <resource_cache.h>

#include "../resources.h" // ResourceGroupID, TechniqueID, MaterialID, GeometryID, Builtin*
#include "vulkan_context.h"

namespace ember::graphics::vulkan {

/**
    * @brief Technique (pipeline) record.
    *
    * Stores compiled pipeline objects and shader modules required to bind a
    * technique at draw time.
    */
struct TechniqueRecord {
    VkPipeline pipeline = VK_NULL_HANDLE;                   ///< Graphics pipeline handle
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;       ///< Pipeline layout
    VkShaderModule vertModule = VK_NULL_HANDLE;             ///< Vertex shader module (retained for hot-reload)
    VkShaderModule fragModule = VK_NULL_HANDLE;             ///< Fragment shader module (retained for hot-reload)

    VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;  ///< Sample count used when creating pipeline
    VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; ///< Primitive topology

    std::uint64_t compatibleRenderPassKey = 0;              ///< Optional key for renderpass/subpass compatibility
};

/**
    * @brief Material record.
    *
    * Materials reference a technique and contain descriptor set layout information
    * and per-frame descriptor sets used for binding material parameters.
    */
struct MaterialRecord {
    TechniqueID technique = BuiltinTechnique::Opaque;       ///< Technique this material uses
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE; ///< Descriptor set layout
    std::vector<VkDescriptorSet> descriptorSetsPerFrame;    ///< Descriptor sets sized to frames-in-flight
    std::uint32_t dynamicOffsetCount = 0;                   ///< Number of dynamic offsets required
    std::size_t pushConstantSize = 0;                       ///< Size of push constant region (bytes)
};

/**
    * @brief Geometry record.
    *
    * Holds vertex/index buffers and related metadata.
    */
struct GeometryRecord {
    VkBuffer vertexBuffer = VK_NULL_HANDLE;                 ///< Device vertex buffer
    VkBuffer indexBuffer = VK_NULL_HANDLE;                  ///< Device index buffer
    std::uint32_t indexCount = 0;                           ///< Number of indices
    VkIndexType indexType = VK_INDEX_TYPE_UINT32;           ///< Index type
    std::uint32_t vertexStride = 0;                         ///< Vertex stride in bytes
};

/**
    * @brief Vulkan resource cache implementation
    */
class VulkanResourceCache : public ResourceCache {
public:
    VulkanResourceCache(const ResourceCacheConfig& config, std::shared_ptr<VulkanContext> context);
    ~VulkanResourceCache() override;

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
        AllocationHandle vertexMemory;           ///< Memory backing vertex buffer
        AllocationHandle indexMemory;            ///< Memory backing index buffer
        bool uploaded = false;                   ///< True if resident on device
    };

    ResourceCacheConfig config_;

    std::shared_ptr<VulkanContext> context_;

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

} // namespace ember::graphics::vulkan