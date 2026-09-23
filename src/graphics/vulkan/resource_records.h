#pragma once

#pragma once

#include "../resources.h" // ResourceGroupID, TechniqueID, MaterialID, GeometryID, Builtin*
#include <vulkan/vulkan.h>
#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>

namespace ember::graphics::vulkan {

/**
 * @file resource_records.h
 * @brief Vulkan backend resource record types used for caching compiled GPU objects.
 *
 * These plain-old-data records hold Vulkan handles and small metadata used by
 * the resource cache and renderer. Creation / destruction of Vulkan handles
 * should be performed by `VulkanContext` / `VulkanResourceCache` helpers; the
 * renderer stores the resulting handles here for binding and lifetime tracking.
 */


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

    /** @note If using a memory allocator (VMA), store allocator handles instead
     *        of raw VkDeviceMemory here and adapt destroy/upload helpers accordingly.
     */
};

} // namespace ember::graphics::vulkan