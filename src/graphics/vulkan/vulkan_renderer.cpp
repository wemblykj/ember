#include "vulkan_renderer.h"

#include <logger.h>

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

bool VulkanRenderer::initialize(platform::SurfaceProvider* provider) {
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
        if (!vulkanProvider->createSurface(context_->getInstance(), surface_)) {
            EMBER_LOG_ERROR("Failed to create Vulkan surface");
            return false;
        }
    }
    else {
        EMBER_LOG_WARN("SurfaceProvider is null, skipping surface creation");
    }

    createFrameResources();

    EMBER_LOG_INFO("Vulkan renderer initialized");
    return true;
}

void VulkanRenderer::shutdown() {
    using namespace ember::core;

    destroyFrameResources();

    if (surface_ != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(context_->getInstance(), surface_, nullptr);
        surface_ = VK_NULL_HANDLE;
    }

    if (context_) {
        context_->shutdown();
        context_.reset();
    }

    EMBER_LOG_INFO("Vulkan renderer shutdown");
}

void VulkanRenderer::beginFrame() {
    FrameContext& frame = frames_[currentFrameIndex_];

    auto device = context_->getDevice();

    vkWaitForFences(device, 1, &frame.inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &frame.inFlightFence);

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
    FrameContext& frame = frames_[currentFrameIndex_];
    vkEndCommandBuffer(frame.commandBuffer);
}

void VulkanRenderer::present() {
    // Implement present logic
    FrameContext& frame = frames_[currentFrameIndex_];
}

void VulkanRenderer::resizeFramebuffer(uint32_t width, uint32_t height) {
    config_.width = width;
    config_.height = height;
}

bool VulkanRenderer::createFrameResources()
{
    frames_.resize(framesInFlight_);
    for (uint32_t i = 0; i < framesInFlight_; ++i) {
        FrameContext& f = frames_[i];
        // Create a resettable per-frame command pool via VulkanContext helper
        context_->createCommandPool(context_->getGraphicsQueueFamily(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, &f.commandPool);

        // Allocate a primary command buffer from that pool
        context_->allocateCommandBuffers(f.commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1, &f.commandBuffer);

        // Create fence and semaphores (renderer creates/destroys these)
        // vkCreateFence(... &f.inFlightFence)
        // vkCreateSemaphore(... &f.imageAvailable), vkCreateSemaphore(... &f.renderFinished)
    }

    return true;
}

void VulkanRenderer::destroyFrameResources()
{
    for (uint32_t i = 0; i < framesInFlight_; ++i) {
        FrameContext& f = frames_[i];
        // Destroy fence and semaphores
        // vkDestroyFence(... f.inFlightFence)
        // vkDestroySemaphore(... f.imageAvailable), vkDestroySemaphore(... f.renderFinished)
        // Free command buffer and destroy command pool
        context_->freeCommandBuffers(f.commandPool, 1, &f.commandBuffer);
		context_->destroyCommandPool(f.commandPool);
    }
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

RendererPtr createRenderer(const RendererConfig& config, platform::SurfaceProvider* surfaceProvider) {
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
