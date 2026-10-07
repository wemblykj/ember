#pragma once

#include <vulkan/vulkan.h>

#include "../resources.h" // ResourceGroupID, TechniqueID, MaterialID, GeometryID, Builtin*

#include "vulkan_defs.h"
#include <vector>

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

    // VK_NULL_HANDLE if unused by this technique
    VkShaderModule vertModule = VK_NULL_HANDLE;             ///< Vertex shader module (retained for hot-reload)
    VkShaderModule fragModule = VK_NULL_HANDLE;             ///< Fragment shader module (retained for hot-reload)
    VkShaderModule computeModule = VK_NULL_HANDLE;

    VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;  ///< Sample count used when creating pipeline
    VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; ///< Primitive topology

    std::uint64_t compatibleRenderPassKey = 0;              ///< Optional key for renderpass/subpass compatibility

    /// Descriptor set layout materials using this technique must allocate against
    /// (typically set=1, reserving set=0 for global/per-frame data).
	VkDescriptorSetLayout materialDescriptorSetLayout = VK_NULL_HANDLE;
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
 * @brief Allocator-specific handles produced alongside a GeometryRecord.
 *
 * Kept separate from GeometryRecord so the public record stays
 * allocator-neutral; only the resource cache consumes these.
 */
struct GeometryAllocation {
    AllocationHandle vertexMemory;
    AllocationHandle indexMemory;
};

/**
 * @brief Compiles backend-agnostic resource descriptions into Vulkan GPU objects.
 *
 * Implementations handle shader compilation (e.g. via shaderc), pipeline
 * creation, descriptor layout generation, and buffer/image uploads. The
 * resource cache calls this interface during registration; it does not
 * perform compilation itself.
 */
class VulkanResourceCompiler {
public:
    virtual ~VulkanResourceCompiler() = default;

    virtual bool compileTechnique(const TechniqueDesc& desc, TechniqueRecord& outRecord) = 0;
    virtual bool compileMaterial(const MaterialDesc& desc, const TechniqueRecord& technique, MaterialRecord& outRecord) = 0;
    virtual bool compileGeometry(const GeometryDesc& desc, GeometryRecord& outRecord, GeometryAllocation& allocation) = 0;

    virtual void destroyTechnique(TechniqueRecord& record) = 0;
    virtual void destroyMaterial(MaterialRecord& record) = 0;
    virtual void destroyGeometry(GeometryRecord& record, GeometryAllocation& allocation) = 0;
};

} // namespace ember::graphics::vulkan