#pragma once

#include "resources.h"

namespace ember::graphics {

	/// @brief Abstract interface for managing rendering resources (techniques, materials, geometries) in a graphics renderer. Concrete implementations (e.g., VulkanResourceCache, OpenGLResourceCache) should derive from this class and implement the virtual methods.
class ResourceCache {
public:
    virtual ~ResourceCache() = default;

    /// @brief Clear all registered resources (techniques, materials, geometries) and reset the renderer to an empty state. This is useful for hot-reloading or resetting the renderer without destroying it.
    virtual void clearAllResources() = 0;

    /// @brief Create a new resource group with the specified description. Resource groups are used to manage collections of resources (e.g., textures, buffers) that can be bound together for rendering.
    virtual ResourceGroupID createResourceGroup(const ResourceGroupDesc& desc) = 0;

    /// @brief Release a previously created resource group, freeing any associated resources. After calling this method, the specified resource group ID is no longer valid.
    virtual void releaseResourceGroup(ResourceGroupID group) = 0;

    /// @brief Check if a technique with the specified TechniqueID is currently registered and valid. Returns true if the technique exists, false otherwise.
    virtual bool assertTechnique(TechniqueID id) = 0;
    /// @brief Check if a material with the specified MaterialID is currently registered and valid. Returns true if the material exists, false otherwise.
    virtual bool assertMaterial(MaterialID id) = 0;
    /// @brief Check if geometry with the specified GeometryID is currently registered and valid. Returns true if the geometry exists, false otherwise.
    virtual bool assertGeometry(GeometryID id) = 0;

    /// @brief Register a new rendering technique with the specified description. Techniques define the fixed-function and shader state used for rendering. Returns a unique TechniqueID that can be used to reference this technique in materials and render passes.
    virtual TechniqueID registerTechnique(const TechniqueDesc& desc, ResourceGroupID group = BuiltinResourceGroup::Default) = 0;
    /// @brief Register a new material with the specified description. Materials reference techniques and provide parameter bindings for rendering. Returns a unique MaterialID that can be used in render packets.
    virtual MaterialID registerMaterial(const MaterialDesc& desc, ResourceGroupID group = BuiltinResourceGroup::Default) = 0;
    /// @brief Register new geometry data (vertex/index buffers) with the specified description. Returns a unique GeometryID that can be used in render packets.
    virtual GeometryID registerGeometry(const GeometryDesc& desc, ResourceGroupID group = BuiltinResourceGroup::Default) = 0;

    /// @brief Set the default material to be used when a render packet does not specify a material. This is useful for ensuring that all geometry is rendered with a valid material, even if the user forgets to assign one.
    /// @return The previous default material ID, which can be restored later if needed.
    virtual MaterialID setUnassignedMaterial(MaterialID id) = 0;
    /// @brief Set the material to be used when a render packet specifies an unresolved material. This is useful for handling cases where a material reference is invalid or missing, allowing the renderer to fall back to a default material instead of failing.
    /// @return The previous default material ID, which can be restored later if needed.
    virtual MaterialID setUnresolvedMaterial(MaterialID id) = 0;
};

/**
* Resource cache creation configuration
*/
struct ResourceCacheConfig {
};

}  // namespace ember::graphics
