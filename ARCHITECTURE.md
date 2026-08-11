# Ember Graphics Engine - Architecture Documentation

## Overview

Ember is built around a **layered architecture** that provides clean separation of concerns. This document explains the design philosophy and how components interact.

## Layer Model

### Layer 1: Application Layer
**Location**: `src/application/`

The application layer contains user-facing code and application logic. It has:
- No direct knowledge of Vulkan, SDL2, or platform specifics
- Dependencies only on the Graphics Interface layer
- A main loop that interacts with the renderer and window abstractions

**Key Files**:
- `main.cpp` - Entry point and main loop

### Layer 2: Graphics Interface
**Location**: `src/graphics/interface/`

The graphics interface defines abstract, graphics-API-agnostic contracts. This is what the Application layer sees.

**Key Abstractions**:
- `GraphicsContext` - Manages GPU device, queues, and lifecycle
- `Renderer` - High-level rendering interface
- `Pipeline` - Shader pipeline abstraction
- `Buffer` - GPU memory abstraction
- `Image` - Texture/image abstraction

**Design Principle**: These are pure virtual interfaces with no implementation details.

```cpp
class Renderer {
public:
    virtual ~Renderer() = default;
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;
    virtual void clear(float r, float g, float b, float a) = 0;
    // ... more abstract methods
};
```

### Layer 3: Graphics Backend
**Location**: `src/graphics/vulkan/`

The Vulkan backend implements the Graphics Interface using Vulkan-specific code.

**Key Classes**:
- `VulkanContext` - Implements `GraphicsContext`
- `VulkanRenderer` - Implements `Renderer`
- `VulkanUtils` - Vulkan helper functions (shader compilation, etc.)

**Design Principle**: All Vulkan code is isolated here. No Vulkan headers leak into higher layers.

```cpp
class VulkanRenderer : public Renderer {
public:
    void beginFrame() override { /* Vulkan impl */ }
    void endFrame() override { /* Vulkan impl */ }
    void clear(float r, float g, float b, float a) override { /* Vulkan impl */ }
    // ...
private:
    VkDevice device_;
    VkCommandBuffer cmdBuffer_;
    // ... Vulkan-specific members
};
```

### Layer 4: Platform Layer
**Location**: `src/platform/`

The platform layer abstracts away OS/windowing system specifics.

**Key Abstractions**:
- `Window` - Window creation and management interface
- `InputHandler` - Input event handling interface

**Current Implementation**: SDL2 in `src/platform/sdl2/`

**Design Principle**: Platform-specific code isolated. Easy to swap SDL2 for GLFW, native APIs, or web canvas.

```cpp
class Window {
public:
    virtual ~Window() = default;
    virtual bool shouldClose() = 0;
    virtual void pollEvents() = 0;
    virtual std::pair<int, int> getSize() = 0;
    // ... more abstract methods
};
```

### Layer 5: Core Utilities
**Location**: `src/core/`

Shared utilities used across layers:
- `Logger` - Logging system
- `Math` - Vector, matrix utilities
- Memory helpers, etc.

## Dependency Flow

```
┌──────────────────────────────────┐
│   Application Layer              │
│   (No platform/graphics knowledge)
└────────────────┬──────────────────┘
                 │
                 ▼
┌──────────────────────────────────┐
│   Graphics Interface             │
│   (Abstract APIs)                │
└────────┬─────────────────────────┘
         │
    ┌────┴────────────┐
    ▼                 ▼
┌─────────────┐   ┌──────────────┐
│   Vulkan    │   │  Platform    │
│  Backend    │   │   (SDL2)     │
└─────────────┘   └──────────────┘
    │                 │
    └────────┬────────┘
             ▼
┌──────────────────────────────────┐
│   Core Utilities                 │
│   (Logging, Math, etc.)          │
└──────────────────────────────────┘
```

**Key Rule**: Dependencies point DOWNWARD only. No upward dependencies.

## Extension Points

### Adding a New Graphics Backend

To add a new backend (e.g., Direct3D, Metal):

1. Create `src/graphics/direct3d/` directory
2. Implement the `Renderer` interface
3. Add a factory function or conditional compilation
4. Application code needs NO changes

Example:
```cpp
#ifdef USE_VULKAN
#include "graphics/vulkan/vulkan_renderer.h"
using RendererImpl = VulkanRenderer;
#elif USE_D3D12
#include "graphics/direct3d/d3d12_renderer.h"
using RendererImpl = D3D12Renderer;
#endif

auto renderer = std::make_unique<RendererImpl>(context);
```

### Adding a New Platform

To replace SDL2 with GLFW or native APIs:

1. Create `src/platform/glfw/` directory
2. Implement the `Window` interface
3. Add conditional compilation
4. Application code needs NO changes

## OpenXR Integration Path

When adding OpenXR support:

1. Create `src/openxr/` layer between Application and Graphics Interface
2. OpenXR layer handles:
   - XR session management
   - Eye tracking and controller input
   - Stereoscopic rendering setup
   - Pose synchronization
3. Minimal changes to graphics interface (stereo rendering support)
4. Application sees same high-level API

## Memory and Lifecycle Management

### Graphics Context Lifecycle
1. Created before any rendering
2. Passed to Renderer during construction
3. Kept alive while rendering
4. Destroyed last (RAII)

### Resource Ownership
- `Renderer` owns GPU resources internally
- Resources exposed via opaque handles or smart pointers
- No direct GPU object access from Application layer

## Building and Compiling

The CMake system ensures:
- Platform/implementation selection at build time
- No circular dependencies
- Proper include paths per layer
- Easy enabling/disabling of features

### Conditional Compilation
```cmake
if(USE_VULKAN)
    target_sources(graphics PRIVATE src/graphics/vulkan/...)
endif()

if(USE_SDL2)
    target_sources(platform PRIVATE src/platform/sdl2/...)
endif()
```

## Testing Strategy

Each layer can be tested independently:

1. **Core Utilities**: No dependencies, easy unit tests
2. **Platform Layer**: Mock Window interface in tests
3. **Graphics Backend**: Mock graphics context
4. **Graphics Interface**: Already abstract, define test doubles
5. **Application**: Mock renderer and window

## Performance Considerations

- **Zero-overhead abstractions**: Virtual calls only at subsystem boundaries
- **Inlining**: Math utilities are header-only
- **No runtime checks**: Use compile-time polymorphism where performance-critical
- **Lazy initialization**: Resources created on-demand

## Future Enhancements

- [ ] Multiple graphics backends
- [ ] Hot-reloading of shaders
- [ ] Scene graph abstraction
- [ ] Physics engine integration
- [ ] Audio subsystem
- [ ] Networking layer
- [ ] Asset management system
