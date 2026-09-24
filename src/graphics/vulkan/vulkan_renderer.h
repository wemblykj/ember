#pragma once

#include "../renderer.h"

#include <memory>
#include <vector>

#include "vulkan_context.h"
#include "vulkan_resource_cache.h"

namespace ember::graphics::vulkan {

/**
 * @brief Vulkan renderer implementation
 */
class VulkanRenderer : public Renderer {
    struct FrameContext {
        uint32_t swapchainImageIndex;
        VkCommandPool commandPool = VK_NULL_HANDLE;
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        VkFence inFlightFence = VK_NULL_HANDLE;
        VkSemaphore imageAvailable = VK_NULL_HANDLE;
        VkSemaphore renderFinished = VK_NULL_HANDLE;
    };

public:
    explicit VulkanRenderer(const RendererConfig& config, std::shared_ptr<VulkanContext> context, std::shared_ptr<VulkanResourceCache> resourceCache);
    ~VulkanRenderer() override;
    
    bool initialize(SurfaceProvider* provider) override;
    void shutdown() override;
    void aquireNextImage(FrameContext& frame, VkDevice device);

    ResourceCache& getResourceCache() override { return *resourceCache_; }

    void beginFrame() override;
    void submitPass(const RendererPass& pass) override;
    void endFrame() override;
    void present() override;
    void resizeFramebuffer(uint32_t width, uint32_t height) override;
    
private:
    std::vector<const char*> getRequiredExtensions();
    VkSurfaceKHR getSurface() const { return surface_; }

    bool createSwapchain();
    void destroySwapchain();
    void recreateSwapchain();
    bool createFrameResources();
    void destroyFrameResources();

    VkSurfaceFormatKHR chooseSurfaceFormat(std::vector<VkSurfaceFormatKHR> formats, 
    	VkFormat preferredFormat = VK_FORMAT_UNDEFINED, VkColorSpaceKHR preferredColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR);
    VkPresentModeKHR choosePresentMode(std::vector<VkPresentModeKHR> presentModes, 
    	VkPresentModeKHR preferredMode = VK_PRESENT_MODE_FIFO_KHR);
    VkExtent2D chooseSurfaceExtent(VkSurfaceCapabilitiesKHR capabilities);
    uint32_t chooseSurfaceImageCount(VkSurfaceCapabilitiesKHR capabilities, uint32_t preferredCount = 2);

private:

    bool initialized_ = false;
    std::shared_ptr<VulkanContext> context_;
    VkSurfaceKHR surface_ = VK_NULL_HANDLE;
    VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
    VkFormat swapchainImageFormat_;
	VkExtent2D swapchainExtent_;

    std::vector<VkImage> swapchainImages_;
    std::vector<VkImageView> swapchainImageViews_;

    std::vector<FrameContext> frames_;
    std::vector<VkFence> imagesInFlight_;
    uint32_t currentFrameIndex_ = 0;
    uint32_t framesInFlight_ = 2;

    std::shared_ptr<VulkanResourceCache> resourceCache_;
    RendererConfig config_;
};

/**
 * Factory function for creating Vulkan renderers
 */
RendererPtr createRenderer(const RendererConfig& config, SurfaceProvider* provider);

}  // namespace ember::graphics::vulkan
