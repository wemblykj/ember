#pragma once

#include "../core/math.h"
#include <memory>
#include <vector>

namespace ember::graphics {

/**
 * Abstract renderer interface.
 * Implementations (Vulkan, DX12, Metal, etc.) inherit from this.
 */
class Renderer {
public:
    virtual ~Renderer() = default;

    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual void beginFrame() = 0;
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
};

}  // namespace ember::graphics
