#pragma once

#include <vulkan/vulkan.h>

#include <resource_types.h>

namespace ember::vulkan {

inline VkIndexType toVkIndexType(resources::IndexType type) {
    switch (type) {
    case resources::IndexType::UInt16: return VK_INDEX_TYPE_UINT16;
    case resources::IndexType::UInt32: return VK_INDEX_TYPE_UINT32;
    }
    return VK_INDEX_TYPE_UINT32; // unreachable if exhaustively handled
}

inline VkFormat toVkVertexFormat(resources::VertexFormat format) {
    switch (format) {
    case resources::VertexFormat::R32G32_SFLOAT: return VK_FORMAT_R32G32_SFLOAT;
    case resources::VertexFormat::R32G32B32_SFLOAT: return VK_FORMAT_R32G32B32_SFLOAT;
    case resources::VertexFormat::R32G32B32A32_SFLOAT: return VK_FORMAT_R32G32B32A32_SFLOAT;
    case resources::VertexFormat::R8G8B8A8_UNORM: return VK_FORMAT_R8G8B8A8_UNORM;
    case resources::VertexFormat::A2B10G10R10_SNORM_PACK32: return VK_FORMAT_A2B10G10R10_SNORM_PACK32;
    }
    return VK_FORMAT_UNDEFINED; // unreachable if exhaustively handled
}

} // namespace ember::vulkan