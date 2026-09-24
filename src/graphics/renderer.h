#pragma once

#include <memory>
#include <vector>

#include <graphics_api.h>

#include "surface_provider.h"
#include "resource_cache.h"
#include "render_packet.h"

namespace ember::graphics {

using RenderQueue = std::vector<RenderPacket>;

struct RendererPass {
    TechniqueID technique = BuiltinTechnique::Opaque;
	SortPolicy sortPolicy = SortPolicy::MaterialMajor;
    RenderQueue queue;
};

/// @brief Sorts the render packets in the given RendererPass according to its specified sorting policy. This function is a convenience wrapper around sortPass(RenderQueue&, SortPolicy) that operates directly on a RendererPass object.
inline void sortPass(RendererPass& pass) {
	sortQueue(pass.queue, pass.sortPolicy);
}

/// @brief Abstract interface for a graphics renderer. Concrete implementations (e.g., VulkanRenderer, OpenGLRenderer) should derive from this class and implement the virtual methods.
class Renderer {
public:
    virtual ~Renderer() = default;

    virtual bool initialize(SurfaceProviderPtr surfaceProvider) = 0;
    virtual void shutdown() = 0;

	/// @brief Get the resource cache associated with this renderer. The resource cache manages techniques, materials, and geometries used for rendering. This method returns a pointer to the ResourceCache interface, allowing access to resource management functions.
	virtual ResourceCache& getResourceCache() = 0;
	
    virtual void beginFrame() = 0;

    /// @brief Submit a rendering pass to the renderer. 
    /// Packets are consumed in the order they appear in pass.queue - the renderer does not sort them. 
    /// Call sortPass(pass) beforehand to order packets according to pass.sortPolicy.
    virtual void submitPass(const RendererPass& pass) = 0;
	
	/// @brief Submit multiple rendering passes to the renderer. Each pass will be processed in the order they appear in the vector. This is a convenience method that calls submitPass(const RendererPass&) for each pass in the vector.
    void submitPass(const std::vector<RendererPass>& passes) {
        for (const auto& pass : passes) {
            submitPass(pass);
        }
    }

    virtual void endFrame() = 0;
    virtual void present() = 0;
    virtual void resizeFramebuffer(uint32_t width, uint32_t height) = 0;
};

using RendererPtr = std::unique_ptr<Renderer>;

/**
 * Renderer creation configuration
 */
struct RendererConfig {
    uint32_t width = 1280;
    uint32_t height = 720;
    bool enableValidation = true;
    bool vsync = true;
	GraphicsAPI api = GraphicsAPI::Default;
};

}  // namespace ember::graphics
