#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <ember_math.h>

#include <graphics_api.h>
#include <surface_provider.h>

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

    virtual bool initialize(platform::SurfaceProvider* provider) = 0;
    virtual void shutdown() = 0;

	/// @brief Clear all registered resources (techniques, materials, geometries) and reset the renderer to an empty state. This is useful for hot-reloading or resetting the renderer without destroying it.
    virtual void clearAllResources() = 0;

	/// @brief Create a new resource group with the specified description. Resource groups are used to manage collections of resources (e.g., textures, buffers) that can be bound together for rendering.
    virtual ResourceGroupID createResourceGroup(const ResourceGroupDesc& desc) = 0;
	/// @brief Release a previously created resource group, freeing any associated resources. After calling this method, the specified resource group ID is no longer valid.
    virtual void releaseResourceGroup(ResourceGroupID group) = 0;

	/// @brief Register a new rendering technique with the specified description. Techniques define the fixed-function and shader state used for rendering. Returns a unique TechniqueID that can be used to reference this technique in materials and render passes.
    virtual TechniqueID registerTechnique(const TechniqueDesc& desc, ResourceGroupID group = BuiltinResourceGroup::Default) = 0;
	/// @brief Register a new material with the specified description. Materials reference techniques and provide parameter bindings for rendering. Returns a unique MaterialID that can be used in render packets.
    virtual MaterialID registerMaterial(const MaterialDesc& desc, ResourceGroupID group = BuiltinResourceGroup::Default) = 0;
	/// @brief Register new geometry data (vertex/index buffers) with the specified description. Returns a unique GeometryID that can be used in render packets.
    virtual GeometryID registerGeometry(const GeometryDesc& desc, ResourceGroupID group = BuiltinResourceGroup::Default) = 0;
    
	/// @brief Unregister a previously registered technique, freeing any associated resources. After calling this method, the specified TechniqueID is no longer valid.
    virtual void unregisterTechnique(TechniqueID id) = 0;
	/// @brief Unregister a previously registered material, freeing any associated resources. After calling this method, the specified MaterialID is no longer valid.
    virtual void unregisterMaterial(MaterialID id) = 0;
	/// @brief Unregister a previously registered geometry, freeing any associated resources. After calling this method, the specified GeometryID is no longer valid.
    virtual void unregisterGeometry(GeometryID id) = 0;

    virtual void beginFrame() = 0;

    /// @brief Submit a rendering pass to the renderer. The pass contains a technique
	/// and a queue of render packets to be drawn. Packets are consumed in the order
	/// they appear in pass.queue — the renderer does not sort them. Call sortPass(pass)
	/// beforehand to order packets according to pass.sortPolicy.
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
