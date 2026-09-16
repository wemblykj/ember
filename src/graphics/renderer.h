#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <ember_math.h>

#include <graphics_api.h>

namespace ember::graphics {
	
using ResourceGroupID = uint32_t;
using SortKey = uint64_t;
using MaterialID = uint32_t;
using GeometryID = uint32_t;
using TechniqueID = uint32_t;

/// @brief Built-in resource group IDs for common rendering resource groups.
namespace BuiltinResourceGroup {
    constexpr ResourceGroupID Default = 0;
	/// @brief Base value for custom resource groups. User-defined resource groups should start from this value to avoid conflicts with built-in resource groups.
    constexpr ResourceGroupID Custom = 0x8000;
}

/// @brief Built-in technique IDs for common rendering techniques.
namespace BuiltinTechnique {
    /// @brief Built-in technique IDs for common rendering techniques.
    constexpr TechniqueID Opaque = 0;
    constexpr TechniqueID Transparent = 1;
    /// @brief Base value for custom techniques. User-defined techniques should start from this value to avoid conflicts with built-in techniques.
    constexpr TechniqueID Custom = 0x8000;
}
	
/// @brief Built-in material IDs for common rendering materials.
namespace BuiltinMaterial {
    constexpr MaterialID Undefined = 0;
	/// @brief Base value for custom materials. User-defined materials should start from this value to avoid conflicts with built-in materials.
    constexpr MaterialID Custom = 0x8000;
}

/// @brief Built-in geometry IDs for common rendering geometries.
namespace BuiltinGeometry {
    constexpr GeometryID FullscreenQuad = 1;
	/// @brief Base value for custom geometries. User-defined geometries should start from this value to avoid conflicts with built-in geometries.
    constexpr GeometryID Custom = 0x8000;
}

/// @brief Generic, backend-agnostic shader source reference.
struct ShaderStageDesc {
    std::string entryPoint = "main";
    std::string sourcePath;   // e.g. "shaders/opaque.vert" (compiled per-backend as needed)
};

enum class BlendMode : uint8_t { Opaque, AlphaBlend, Additive };
enum class CullMode : uint8_t { None, Front, Back };

/// @brief Backend-agnostic description of a resource group, which can contain multiple resources (e.g., textures, buffers) that can be bound together for rendering.
struct ResourceGroupDesc {
    std::string debugName; // for logging/tooling only, not used for lookup
};

/// @brief Backend-agnostic description of a technique's fixed-function + shader state.
struct TechniqueDesc {
    std::string name;
    ShaderStageDesc vertexShader;
    ShaderStageDesc fragmentShader;
    BlendMode blendMode = BlendMode::Opaque;
    CullMode cullMode = CullMode::Back;
    bool depthTest = true;
    bool depthWrite = true;
};

/// @brief Backend-agnostic description of a material, which references a technique and provides parameter bindings.
struct MaterialDesc {
    std::string name;
    TechniqueID technique;              // which technique this material uses
    // parameter bindings: textures, uniform values, etc.
    // (deliberately generic — actual binding layout is backend-specific)
};

/// @brief Backend-agnostic description of geometry data (vertex/index buffers).
struct GeometryDesc {
    // Backend-agnostic description — actual vertex format/layout details
    // will depend on how you want to structure vertex attributes.
    const void* vertexData = nullptr;
    size_t vertexDataSize = 0;
    const void* indexData = nullptr;
    size_t indexDataSize = 0;
    uint32_t indexCount = 0;
    // vertex layout descriptor, stride, etc. — fill in as your vertex format design solidifies
};

/// @brief A single render packet, representing a draw call with associated transform, material, and geometry.
struct RenderPacket {
    core::Mat4 transform;
    MaterialID materialId;
    GeometryID geometryId;
    SortKey sortKey;
};

/// @brief Sorting policy for render packets within a pass. Determines how packets are ordered before rendering.
/// - MaterialMajor: group by material first (minimize state changes), useful
///   for opaque geometry where draw order otherwise doesn't affect correctness.
/// - DepthMajor: sort strictly by depth(typically back - to - front), required
///   for correct alpha blending; material changes are tolerated as needed.
enum class SortPolicy : uint8_t {
    MaterialMajor,
    DepthMajor
};

using RenderQueue = std::vector<RenderPacket>;

struct RendererPass {
    TechniqueID technique = BuiltinTechnique::Opaque;
	SortPolicy sortPolicy = SortPolicy::MaterialMajor;
    RenderQueue queue;
};

/// @brief Sorts the render packets in the given queue according to the specified sorting policy. This function is intended to be called before submitting a pass to the renderer, ensuring that packets are ordered correctly for rendering.
inline void sortPass(RenderQueue& queue, SortPolicy sortPolicy) {
    if (sortPolicy == SortPolicy::MaterialMajor) {
        std::sort(queue.begin(), queue.end(),
            [](const RenderPacket& a, const RenderPacket& b) {
                if (a.materialId != b.materialId) return a.materialId < b.materialId;
                return a.sortKey < b.sortKey;
            });
    }
    else { // DepthMajor
        std::sort(queue.begin(), queue.end(),
            [](const RenderPacket& a, const RenderPacket& b) {
                return a.sortKey > b.sortKey; // back-to-front
            });
    }
}

/// @brief Sorts the render packets in the given RendererPass according to its specified sorting policy. This function is a convenience wrapper around sortPass(RenderQueue&, SortPolicy) that operates directly on a RendererPass object.
inline void sortPass(RendererPass& pass) {
	sortPass(pass.queue, pass.sortPolicy);
}

/// @brief Abstract interface for a graphics renderer. Concrete implementations (e.g., VulkanRenderer, OpenGLRenderer) should derive from this class and implement the virtual methods.
class Renderer {
public:
    virtual ~Renderer() = default;

    virtual bool initialize() = 0;
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
