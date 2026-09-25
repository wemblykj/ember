#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <set>
#include <vector>

#include <vulkan/vulkan.h>

#include "any_physical_device_selector.h"
#include "vulkan_defs.h"
#include "physical_device_selector.h"

namespace ember::graphics::vulkan {

/**
 * @brief Structure representing an allocation handle for Vulkan resources. This structure contains information about the backend used for memory allocation and an opaque handle to the allocated memory.
 */
struct AllocationHandle {
    enum class Backend : uint8_t { None = 0, Vma, Vk } backend = Backend::None; uintptr_t handle = 0; // opaque storage for backend-specific handle
};

/**
* @brief Abstract base class representing a Vulkan context. This class provides an interface for managing Vulkan resources such as instances, devices, command pools, and buffers.
*/
class VulkanContext {
public:
    virtual ~VulkanContext() = default;

    virtual bool createDefaultInstance(VkInstance& instance, const ExtensionSet& requiredExtensions = {}) = 0;
    virtual bool initialize(VkInstance instance, PhysicalDeviceSelectorPtr deviceSelector = nullptr) = 0;
    virtual bool initialize(const ExtensionSet& requiredExtensions = {}, PhysicalDeviceSelectorPtr deviceSelector = nullptr) = 0;
    virtual void destroy() = 0;

    virtual VkInstance getInstance() const = 0;
    virtual VkPhysicalDevice getPhysicalDevice() const = 0;
    virtual VkDevice getDevice() const = 0;
    virtual VkQueue getGraphicsQueue() const = 0;
    virtual uint32_t getGraphicsQueueFamily() const = 0;

	/**
	 * @brief Waits for the device to become idle. This function blocks until all submitted
	 * commands have been completed.
	 */
    virtual void waitIdle() = 0;

    /**
    * @brief Creates a command pool for the specified queue family index and flags. The created command pool is returned in outPool.
    * @param queueFamilyIndex The index of the queue family for which to create the command pool.
    * @param flags Command pool creation flags.
    * @param outPool Pointer to the variable that will receive the created command pool.
    * @return VkResult indicating success or failure.
    */
    virtual VkResult createCommandPool(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags, VkCommandPool* outPool) = 0;

    /**
    * @brief Destroys the specified command pool.
    * @param pool The command pool to destroy.
    */
    virtual void destroyCommandPool(VkCommandPool pool) = 0;

    /**
    * @brief Allocates command buffers from the specified command pool.
    * @param pool The command pool from which to allocate command buffers.
    * @param level The level of the command buffers (primary or secondary).
    * @param count The number of command buffers to allocate.
    * @param outBuffers Pointer to the array that will receive the allocated command buffers.
    * @return VkResult indicating success or failure.
    */
    virtual VkResult allocateCommandBuffers(VkCommandPool pool, VkCommandBufferLevel level, uint32_t count, VkCommandBuffer* outBuffers) = 0;

    /**
    * @brief Frees command buffers back to the specified command pool.
    * @param pool The command pool from which to free command buffers.
    * @param count The number of command buffers to free.
    * @param buffers Pointer to the array of command buffers to free.
    */
    virtual void freeCommandBuffers(VkCommandPool pool, uint32_t count, const VkCommandBuffer* buffers) = 0;

    /**
    * @brief Begins recording a one-time use command buffer.
    * @return The command buffer that is being recorded.
    */
    virtual VkCommandBuffer beginOneTimeCommands() = 0;

    /**
    * @brief Ends recording of a one-time use command buffer and submits it to the graphics queue.
    * @param cmd The command buffer to end and submit.
    */
    virtual void endOneTimeCommands(VkCommandBuffer cmd) = 0;

    /**
    * @brief Creates a buffer with the specified size and usage flags.
    * @param size The size of the buffer in bytes.
    * @param usage The usage flags for the buffer.
    * @param outBuffer Pointer to the variable that will receive the created buffer.
    * @param outMemory Pointer to the variable that will receive the allocated memory for the buffer.
    * @return VkResult indicating success or failure.
    */
    virtual VkResult createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer* outBuffer, AllocationHandle* outMemory) = 0;

    /**
    * @brief Destroys the specified buffer and frees its associated memory.
    * @param buffer The buffer to destroy.
    * @param memory The memory to free.
    */
    virtual void destroyBuffer(VkBuffer buffer, AllocationHandle memory) = 0;

    virtual VkResult createImage(VkDeviceSize size, VkImageType type, VkImageUsageFlags usage, VkImage* outImage, AllocationHandle* outMemory) = 0;
};

using VulkanContextPtr = std::shared_ptr<VulkanContext>;

}  // namespace ember::graphics::vulkan
