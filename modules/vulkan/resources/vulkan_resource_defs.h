#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include <resource_types.h>
#include <resource_descriptors.h>

#include "../vulkan_defs.h"

namespace ember::vulkan {

/**
 * @brief Structure representing an allocation handle for Vulkan resources. This structure contains information about the backend used for memory allocation and an opaque handle to the allocated memory.
 */
struct AllocationHandle {
    enum class Backend : uint8_t { None = 0, Vma, Vk } backend = Backend::None; uintptr_t handle = 0; // opaque storage for backend-specific handle
};

struct ShaderRecord {
	VkShaderModule module = VK_NULL_HANDLE; ///< Vulkan shader module handle
	VkShaderStageFlagBits type = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM; ///< Shader stage (vertex, fragment, etc.)
};

/**
    * @brief Technique (pipeline) record.
    *
    * Stores compiled pipeline objects and shader modules required to bind a
    * technique at draw time.
    */
struct TechniqueRecord {
    VkPipeline pipeline = VK_NULL_HANDLE;                   ///< Graphics pipeline handle
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;       ///< Pipeline layout

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
    resources::TechniqueID technique = resources::CannedTechnique::Opaque;       ///< Technique this material uses
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

struct VertexFormatRecord {
    std::uint32_t stride = 0;                                   ///< Vertex stride in bytes
    std::vector<VkVertexInputAttributeDescription> attributes;  ///< Vertex attribute descriptions
};

} // namespace ember::vulkan