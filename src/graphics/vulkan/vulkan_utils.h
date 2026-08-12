#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>
#include <cstdint>

namespace ember::graphics::vulkan {

class VulkanContext;

/**
 * @namespace VulkanUtils
 * @brief Common Vulkan utilities shared between Desktop and VR renderers
 */
namespace VulkanUtils {

/**
 * @struct QueueFamilyIndices
 * @brief Indices of queue families with specific capabilities
 */
struct QueueFamilyIndices {
    uint32_t graphicsFamily;
    uint32_t presentFamily;
    bool hasGraphicsFamily = false;
    bool hasPresentFamily = false;

    bool isComplete() const {
        return hasGraphicsFamily && hasPresentFamily;
    }
};

/**
 * @struct Buffer
 * @brief Represents a Vulkan buffer with associated device memory
 */
struct Buffer {
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkDeviceSize size = 0;
};

/**
 * @struct Image
 * @brief Represents a Vulkan image with view and associated device memory
 */
struct Image {
    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkImageView imageView = VK_NULL_HANDLE;
    VkFormat format = VK_FORMAT_UNDEFINED;
    uint32_t width = 0;
    uint32_t height = 0;
};

// Queue family utilities
/**
 * Find queue families with specific capabilities
 * @param physicalDevice Physical device to query
 * @param surface Surface to check presentation support (optional)
 * @return QueueFamilyIndices with found queue families
 */
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice, 
                                      VkSurfaceKHR surface = VK_NULL_HANDLE);

// Memory utilities
/**
 * Find memory type index that satisfies the requirements
 * @param physicalDevice Physical device
 * @param typeFilter Bitmask of memory types to consider
 * @param properties Required memory properties
 * @return Memory type index, or UINT32_MAX if not found
 */
uint32_t findMemoryType(VkPhysicalDevice physicalDevice,
                       uint32_t typeFilter,
                       VkMemoryPropertyFlags properties);

// Buffer utilities
/**
 * Create a Vulkan buffer with automatic memory allocation
 * @param context Shared Vulkan context
 * @param size Buffer size in bytes
 * @param usage Buffer usage flags
 * @param properties Memory property flags
 * @return Buffer object with allocated memory
 */
Buffer createBuffer(std::shared_ptr<VulkanContext> context,
                   VkDeviceSize size,
                   VkBufferUsageFlags usage,
                   VkMemoryPropertyFlags properties);

/**
 * Copy data to a buffer
 * @param context Shared Vulkan context
 * @param dst Destination buffer
 * @param src Source data pointer
 * @param size Number of bytes to copy
 */
void copyToBuffer(std::shared_ptr<VulkanContext> context,
                 const Buffer& dst,
                 const void* src,
                 VkDeviceSize size);

/**
 * Destroy a buffer and free its memory
 * @param context Shared Vulkan context
 * @param buffer Buffer to destroy
 */
void destroyBuffer(std::shared_ptr<VulkanContext> context, Buffer& buffer);

// Image utilities
/**
 * Create a Vulkan image with automatic memory allocation
 * @param context Shared Vulkan context
 * @param width Image width
 * @param height Image height
 * @param format Image format
 * @param tiling Image tiling
 * @param usage Image usage flags
 * @param properties Memory property flags
 * @return Image object with view and allocated memory
 */
Image createImage(std::shared_ptr<VulkanContext> context,
                 uint32_t width,
                 uint32_t height,
                 VkFormat format,
                 VkImageTiling tiling,
                 VkImageUsageFlags usage,
                 VkMemoryPropertyFlags properties);

/**
 * Create an image view for an existing image
 * @param context Shared Vulkan context
 * @param image Vulkan image handle
 * @param format Image format
 * @param aspectFlags Image aspect flags
 * @return VkImageView handle
 */
VkImageView createImageView(std::shared_ptr<VulkanContext> context,
                           VkImage image,
                           VkFormat format,
                           VkImageAspectFlags aspectFlags);

/**
 * Transition image layout
 * @param context Shared Vulkan context
 * @param image Image to transition
 * @param format Image format
 * @param oldLayout Current layout
 * @param newLayout Target layout
 */
void transitionImageLayout(std::shared_ptr<VulkanContext> context,
                          VkImage image,
                          VkFormat format,
                          VkImageLayout oldLayout,
                          VkImageLayout newLayout);

/**
 * Copy buffer to image
 * @param context Shared Vulkan context
 * @param buffer Source buffer
 * @param image Destination image
 * @param width Image width
 * @param height Image height
 */
void copyBufferToImage(std::shared_ptr<VulkanContext> context,
                      VkBuffer buffer,
                      VkImage image,
                      uint32_t width,
                      uint32_t height);

/**
 * Destroy an image and free its memory
 * @param context Shared Vulkan context
 * @param image Image to destroy
 */
void destroyImage(std::shared_ptr<VulkanContext> context, Image& image);

// Format utilities
/**
 * Find a supported format from a list of candidates
 * @param physicalDevice Physical device
 * @param candidates List of candidate formats
 * @param tiling Image tiling
 * @param features Required features
 * @return First supported format, or VK_FORMAT_UNDEFINED if none found
 */
VkFormat findSupportedFormat(VkPhysicalDevice physicalDevice,
                            const std::vector<VkFormat>& candidates,
                            VkImageTiling tiling,
                            VkFormatFeatureFlags features);

/**
 * Check if a format is supported
 * @param physicalDevice Physical device
 * @param format Format to check
 * @param tiling Image tiling
 * @param features Required features
 * @return true if format supports all required features
 */
bool isFormatSupported(VkPhysicalDevice physicalDevice,
                      VkFormat format,
                      VkImageTiling tiling,
                      VkFormatFeatureFlags features);

// Command buffer utilities
/**
 * Create a command pool
 * @param context Shared Vulkan context
 * @param flags Command pool creation flags
 * @return VkCommandPool handle
 */
VkCommandPool createCommandPool(std::shared_ptr<VulkanContext> context,
                               VkCommandPoolCreateFlags flags = 0);

/**
 * Allocate command buffers
 * @param context Shared Vulkan context
 * @param commandPool Command pool to allocate from
 * @param count Number of command buffers to allocate
 * @return Vector of VkCommandBuffer handles
 */
std::vector<VkCommandBuffer> allocateCommandBuffers(
    std::shared_ptr<VulkanContext> context,
    VkCommandPool commandPool,
    uint32_t count);

/**
 * Begin a single-use command buffer
 * @param context Shared Vulkan context
 * @param commandPool Command pool
 * @return VkCommandBuffer ready for recording
 */
VkCommandBuffer beginSingleUseCommandBuffer(
    std::shared_ptr<VulkanContext> context,
    VkCommandPool commandPool);

/**
 * End and submit a single-use command buffer
 * @param context Shared Vulkan context
 * @param commandPool Command pool
 * @param commandBuffer Command buffer to submit
 */
void endSingleUseCommandBuffer(std::shared_ptr<VulkanContext> context,
                              VkCommandPool commandPool,
                              VkCommandBuffer commandBuffer);

}  // namespace VulkanUtils

}  // namespace ember::graphics::vulkan
