#include "vulkan_renderer.h"

#include <logger.h>
#include <map>

#include "vulkan_context_vma.h"
#include "vulkan_surface_provider.h"

namespace ember::graphics::vulkan {

VulkanRenderer::VulkanRenderer(const RendererConfig& config, std::shared_ptr<VulkanContext> context, std::shared_ptr<VulkanResourceCache> resourceCache)
    : config_(config)
    , context_(context)
    , resourceCache_(resourceCache) {
}

VulkanRenderer::~VulkanRenderer() {
    shutdown();
}

bool VulkanRenderer::initialize(SurfaceProvider* provider) {
    using namespace ember::core;

    EMBER_LOG_INFO("Initializing Vulkan renderer...");

    auto vulkanProvider = dynamic_cast<VulkanSurfaceProvider*>(provider);

    if (!vulkanProvider) {
        EMBER_LOG_ERROR("SurfaceProvider is not a VulkanSurfaceProvider");
        return false;
    }

    // Get required extensions
    std::vector<const char*> extensions = getRequiredExtensions();

    // Create surface if window provided (for desktop rendering)
    if (vulkanProvider) {
        std::vector<const char*> surfaceExtensions = vulkanProvider->getRequiredInstanceExtensions();
        extensions.insert(extensions.end(), surfaceExtensions.begin(), surfaceExtensions.end());
    }

	context_->initialize(extensions);

    // Create surface if window provided (for desktop rendering)
    if (vulkanProvider) {
        if (vulkanProvider->createSurface(context_->getInstance(), surface_)) {
            if (!createSwapchain()) {
                EMBER_LOG_ERROR("Failed to create swapchain");
                return false;
			}

            imagesInFlight_.resize(swapchainImages_.size(), VK_NULL_HANDLE);
        }
    }
    else {
        EMBER_LOG_WARN("SurfaceProvider is null, skipping surface creation");
    }

    createFrameResources();

    initialized_ = true;

    EMBER_LOG_INFO("Vulkan renderer initialized");
    return true;
}

void VulkanRenderer::shutdown() {
    using namespace ember::core;
    if (!initialized_) {
        return;
    }

    context_->waitIdle();

    destroyFrameResources();

    if (swapchain_ != VK_NULL_HANDLE) {
        destroySwapchain();
	}

    if (surface_ != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(context_->getInstance(), surface_, nullptr);
        surface_ = VK_NULL_HANDLE;
    }

    if (context_) {
        context_->shutdown();
        context_.reset();
    }

    initialized_ = false;

    EMBER_LOG_INFO("Vulkan renderer shutdown");
}

void VulkanRenderer::aquireNextImage(FrameContext& frame, VkDevice device)
{
    VkResult result = vkAcquireNextImageKHR(device, swapchain_, UINT64_MAX, frame.imageAvailable, VK_NULL_HANDLE, &frame.swapchainImageIndex);

	if (result != VK_SUCCESS) {
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            EMBER_LOG_WARN("Swapchain is out of date or suboptimal, recreating swapchain");
            recreateSwapchain();

            return;
        }

		EMBER_LOG_ERROR("Failed to acquire next swapchain image");
	}

    // If a previous frame is still using this image, wait for it.
    if (imagesInFlight_[frame.swapchainImageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(device, 1, &imagesInFlight_[frame.swapchainImageIndex], VK_TRUE, UINT64_MAX);
    }

    imagesInFlight_[frame.swapchainImageIndex] = frame.inFlightFence;
}

void VulkanRenderer::beginFrame() {
    FrameContext& frame = frames_[currentFrameIndex_];

    auto device = context_->getDevice();

    vkWaitForFences(device, 1, &frame.inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &frame.inFlightFence);

    // request image from the swapchain
    aquireNextImage(frame, device);

    // reset pool to free previous buffers in one call
    vkResetCommandPool(device, frame.commandPool, 0);

    VkCommandBufferBeginInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    info.pInheritanceInfo = nullptr;

    vkBeginCommandBuffer(frame.commandBuffer, &info);
}

void VulkanRenderer::submitPass(const RendererPass& pass) {
	MaterialID currentMaterial = BuiltinMaterial::Undefined;

    for (const auto& packet : pass.queue) {
        if (packet.materialId != currentMaterial) {
            auto material = resourceCache_->ResolveMaterial(packet.materialId);
            currentMaterial = packet.materialId;
        }
        // bind per-draw data, vkCmdDrawIndexed, etc.
    }
}

void VulkanRenderer::endFrame() {
    VkDevice device = context_->getDevice();

    FrameContext& frame = frames_[currentFrameIndex_];
    vkEndCommandBuffer(frame.commandBuffer);

    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &frame.imageAvailable,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = &frame.commandBuffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &frame.renderFinished,
    };

    if (vkQueueSubmit(context_->getGraphicsQueue(), 1, &submitInfo, frame.inFlightFence) != VK_SUCCESS) {
        EMBER_LOG_ERROR("Failed to submit draw command buffer");
    }
}

void VulkanRenderer::present() {
    VkDevice device = context_->getDevice();

    // Implement present logic
    FrameContext& frame = frames_[currentFrameIndex_];

    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &frame.renderFinished,
        .swapchainCount = 1,
        .pSwapchains = &swapchain_,
        .pImageIndices = &frame.swapchainImageIndex
    };
    
    VkResult result = vkQueuePresentKHR(context_->getGraphicsQueue(), &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        EMBER_LOG_WARN("Swapchain is out of date or suboptimal, recreating swapchain");
        recreateSwapchain();
    } else if (result != VK_SUCCESS) {
        EMBER_LOG_ERROR("Failed to present swapchain image");
	}

    currentFrameIndex_ = (currentFrameIndex_ + 1) % framesInFlight_;
}

void VulkanRenderer::resizeFramebuffer(uint32_t width, uint32_t height) {
    config_.width = width;
    config_.height = height;
}


std::vector<const char*> VulkanRenderer::getRequiredExtensions() {
    std::vector<const char*> extensions;

    // Core extensions
    extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

    // Platform-specific surface extension
#ifdef VK_USE_PLATFORM_WIN32_KHR
    extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
    extensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XCBKHR)
    extensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_METAL_EXT)
    extensions.push_back(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
#endif

    return extensions;
}

bool VulkanRenderer::createSwapchain() {
    VkPhysicalDevice physicalDevice = context_->getPhysicalDevice();

	VkSurfaceCapabilitiesKHR capabilities{};
	if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface_, &capabilities) != VK_SUCCESS) {
        EMBER_LOG_ERROR("Failed to get physical device surface capabilities");
        return false;
    }

    uint32_t formatCount = 0;
	if (vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface_, &formatCount, nullptr) != VK_SUCCESS) {
        EMBER_LOG_ERROR("Failed to get physical device surface formats");
        return false;
	}

    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface_, &formatCount, formats.data());

    uint32_t presentModeCount = 0;
	if (vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface_, &presentModeCount, nullptr) != VK_SUCCESS) {
        EMBER_LOG_ERROR("Failed to get physical device surface present modes");
        return false;
    }
    
	std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface_, &presentModeCount, presentModes.data()) != VK_SUCCESS) {
        EMBER_LOG_ERROR("Failed to get physical device surface present modes");
        return false;
    }

    VkSurfaceFormatKHR chosenFormat = chooseSurfaceFormat(formats);

    swapchainImageFormat_ = chosenFormat.format;

    VkPresentModeKHR presentMode = choosePresentMode(presentModes, VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR);
    swapchainExtent_ = chooseSurfaceExtent(capabilities);
    uint32_t imageCount = chooseSurfaceImageCount(capabilities);

    // assume graphics and present on same family. If not, use concurrent sharing mode.
    uint32_t queueFamily = context_->getGraphicsQueueFamily();
    uint32_t queueFamilyIndices[] = { queueFamily, queueFamily };

    VkSwapchainCreateInfoKHR scInfo{};
    scInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    scInfo.surface = surface_;
    scInfo.minImageCount = imageCount;
    scInfo.imageFormat = chosenFormat.format;
    scInfo.imageColorSpace = chosenFormat.colorSpace;
    scInfo.imageExtent = swapchainExtent_;
    scInfo.imageArrayLayers = 1;
    scInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    scInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    scInfo.queueFamilyIndexCount = 0;
    scInfo.pQueueFamilyIndices = nullptr;
    scInfo.preTransform = capabilities.currentTransform;
    scInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    scInfo.presentMode = presentMode;
    scInfo.clipped = VK_TRUE;
    scInfo.oldSwapchain = swapchain_;

    VkDevice device = context_->getDevice();

    VkSwapchainKHR newSwapchain = VK_NULL_HANDLE;
    VkResult res = vkCreateSwapchainKHR(device, &scInfo, nullptr, &newSwapchain);
    if (res != VK_SUCCESS) {
        EMBER_LOG_ERROR("vkCreateSwapchainKHR failed: {}", std::to_string(res));
        return false;
    }

    // If replacing existing swapchain, destroy old resources after creating new one
    if (swapchain_ != VK_NULL_HANDLE) {
        // optionally keep oldSwapchain to pass into new create info for seamless transition
        context_->waitIdle();
        destroySwapchain(); // cleanup previous image views/framebuffers and old swapchain
    }

    swapchain_ = newSwapchain;

    uint32_t actualCount = 0;
    vkGetSwapchainImagesKHR(device, swapchain_, &actualCount, nullptr);
    swapchainImages_.resize(actualCount);
    vkGetSwapchainImagesKHR(device, swapchain_, &actualCount, swapchainImages_.data());

    swapchainImageViews_.resize(actualCount);
    for (uint32_t i = 0; i < actualCount; ++i) {
        VkImageViewCreateInfo ivInfo{};
        ivInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ivInfo.image = swapchainImages_[i];
        ivInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        ivInfo.format = swapchainImageFormat_;
        ivInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
                              VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
        ivInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ivInfo.subresourceRange.baseMipLevel = 0;
        ivInfo.subresourceRange.levelCount = 1;
        ivInfo.subresourceRange.baseArrayLayer = 0;
        ivInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device, &ivInfo, nullptr, &swapchainImageViews_[i]) != VK_SUCCESS) {
            EMBER_LOG_ERROR("Failed to create image view for swapchain image {}", i);
            return false;
        }
    }

    return true;
}

void VulkanRenderer::destroySwapchain() {
    auto device = context_->getDevice();

    // Destroy framebuffers
    /*for (auto fb : swapchainFramebuffers_) {
        if (fb != VK_NULL_HANDLE) {
            vkDestroyFramebuffer(device, fb, nullptr);
        }
    }
    swapchainFramebuffers_.clear();*/

    // Destroy image views
    for (auto iv : swapchainImageViews_) {
        if (iv != VK_NULL_HANDLE) {
            vkDestroyImageView(device, iv, nullptr);
        }
    }
    swapchainImageViews_.clear();

    // Destroy swapchain
    if (swapchain_ != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device, swapchain_, nullptr);
        swapchain_ = VK_NULL_HANDLE;
    }

    swapchainImages_.clear();
}

void VulkanRenderer::recreateSwapchain()
{
	// will destroy old swapchain and create a new one
    createSwapchain();
    imagesInFlight_.resize(swapchainImages_.size(), VK_NULL_HANDLE);
}

bool VulkanRenderer::createFrameResources()
{
	VkDevice device = context_->getDevice();
    uint32_t queueFamilyIndex = context_->getGraphicsQueueFamily();

    VkFenceCreateInfo fenceCreateInfo{
	    .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
	    .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    VkSemaphoreCreateInfo semaphoreCreateInfo{
	    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
	    .flags = 0
    };

    frames_.resize(framesInFlight_);
    for (uint32_t i = 0; i < framesInFlight_; ++i) {
        FrameContext& frame = frames_[i];
        // Create a resettable per-frame command pool via VulkanContext helper
        context_->createCommandPool(queueFamilyIndex, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, &frame.commandPool);

        // Allocate a primary command buffer from that pool
        context_->allocateCommandBuffers(frame.commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1, &frame.commandBuffer);

        // Create fence and semaphores (renderer creates/destroys these)
        vkCreateFence(device, &fenceCreateInfo, nullptr, &frame.inFlightFence);
        vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &frame.imageAvailable);
        vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &frame.renderFinished);
    }

    return true;
}

void VulkanRenderer::destroyFrameResources()
{
    VkDevice device = context_->getDevice();

    for (uint32_t i = 0; i < framesInFlight_; ++i) {
        FrameContext& f = frames_[i];

        // Destroy fence and semaphores
        vkDestroySemaphore(device, f.renderFinished, nullptr);
        vkDestroySemaphore(device, f.imageAvailable, nullptr);
        vkDestroyFence(device, f.inFlightFence, nullptr);
		
        // Free command buffer and destroy command pool
        context_->freeCommandBuffers(f.commandPool, 1, &f.commandBuffer);
		context_->destroyCommandPool(f.commandPool);
    }
}

VkSurfaceFormatKHR VulkanRenderer::chooseSurfaceFormat(std::vector<VkSurfaceFormatKHR> formats, 
	VkFormat preferredFormat, VkColorSpaceKHR preferredColorSpace)
{
    VkSurfaceFormatKHR format = formats[0];
    int bestScore = 0;

	// choose the best format based on preferred format and color space otherwise fallback to a safe default if possible otherwise just pick the first available format
    for (const auto& f : formats) {
        int score = 1;

        if (f.format == preferredFormat) {
            score += 4;
        } else if (f.format == VK_FORMAT_B8G8R8A8_SRGB) {
            score += 2;
        }

        if (f.colorSpace == preferredColorSpace) {
            score += 2;
        } else if (f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            score += 1;
        }

        if (score > bestScore) {
            bestScore = score;
            format = f;
		}
    }

    return format;
}

VkPresentModeKHR VulkanRenderer::choosePresentMode(std::vector<VkPresentModeKHR> presentModes, VkPresentModeKHR preferredMode)
{
    for (const auto& pm : presentModes) {
        if (pm == preferredMode) {
            return pm;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanRenderer::chooseSurfaceExtent(VkSurfaceCapabilitiesKHR capabilities)
{
    VkExtent2D extent;
    if (capabilities.currentExtent.width != UINT32_MAX) {
        extent = capabilities.currentExtent;
    }
    else {
        extent.width = std::clamp(config_.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        extent.height = std::clamp(config_.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    }

    return extent;
}

uint32_t VulkanRenderer::chooseSurfaceImageCount(VkSurfaceCapabilitiesKHR capabilities, uint32_t preferredCount)
{
    return std::clamp(preferredCount, capabilities.minImageCount, capabilities.maxImageCount);
}

RendererPtr createRenderer(const RendererConfig& config, SurfaceProvider* surfaceProvider) {
    auto context = std::make_shared<VulkanContextVma>();

    ResourceCacheConfig resourceCacheConfig;
    auto resourceCache = std::make_shared<VulkanResourceCache>(resourceCacheConfig, context);

    auto renderer = std::make_unique<VulkanRenderer>(config, context, resourceCache);
    if (renderer->initialize(surfaceProvider)) {
        return renderer;
    }
    return nullptr;
}


}  // namespace ember::graphics::vulkan
