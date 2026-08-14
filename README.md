# Ember Engine

A modern C++ graphics engine with Vulkan support and simultaneous Desktop + VR rendering via OpenXR.

## Features

- **Vulkan Rendering**: Modern GPU-accelerated graphics using Vulkan API
- **Shared Vulkan Context**: Single graphics device shared between Desktop and VR renderers
- **Simultaneous Desktop + VR**: Render to monitor and VR headset at the same time
- **OpenXR VR Support**: Cross-platform VR rendering (optional)
- **SDL2 Window Management**: Cross-platform window and input handling
- **Layered Architecture**: Clean separation between core, platform, and graphics layers
- **Sample Applications**: Desktop and VR examples demonstrating engine capabilities

## Dependencies

### Required
- **CMake** 3.20 or higher
- **C++17** compatible compiler (GCC, Clang, MSVC)
- **Vulkan SDK** (1.2 or higher)
- **SDL2** (2.0.9 or higher)
- **GLM** (0.9.9.8 or higher)

### Optional
- **OpenXR SDK** (1.0 or higher) - For VR support

### Installation

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install cmake build-essential vulkan-tools libvulkan-dev libvulkan1 \
                     libsdl2-dev libglm-dev

# Optional: OpenXR SDK
sudo apt-get install libopenxr-dev
```

#### macOS
```bash
brew install cmake vulkan-headers vulkan-loader sdl2 glm

# Optional: OpenXR (requires Khronos SDK or building from source)
```

#### Windows
Download and install:
- [CMake](https://cmake.org/download/)
- [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/)
- [SDL2](https://www.libsdl.org/download-2.0.php)
- [GLM](https://github.com/g-truc/glm/releases)
- [OpenXR SDK](https://github.com/KhronosGroup/OpenXR-SDK) (optional)

## Prerequisites

- **Vulkan SDK**: Download from [LunarG](https://vulkan.lunarg.com/)
  - Windows: Run installer
  - Linux: `sudo apt install vulkan-sdk` or equivalent
  - macOS: Install MoltenVK

## Building

1. Install dependencies: `vcpkg install`
2. Configure: `cmake --preset windows-debug` (or linux-debug)
3. Build: `cmake --build build`

### Desktop Build (Vulkan only)

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Debug
./samples/ember_sample
```

### Desktop + VR Build (With OpenXR)

```bash
mkdir build && cd build
cmake .. -DEMBER_WITH_OPENXR=ON
cmake --build . --config Debug

# Run desktop sample
./samples/ember_sample

# Run VR sample (requires VR headset or OpenXR runtime)
./samples/ember_vr_sample
```

## Project Structure

```
ember/
├── src/
│   ├── core/              # Core utilities (logging, math)
│   ├── platform/          # Platform abstractions (window management)
│   │   └── sdl2/          # SDL2 implementation
│   ├── graphics/          # Graphics layer
│   │   ├── renderer.h     # Abstract renderer interface
│   │   ├── renderer_config.h  # Configuration structs
│   │   ├── vulkan/        # Vulkan renderer
│   │   │   ├── vulkan_context.h      # Shared Vulkan context
│   │   │   ├── vulkan_renderer.h     # Desktop renderer
│   │   │   ├── vulkan_utils.h        # Common utilities
│   │   │   └── ...
│   │   └── openxr/        # OpenXR VR renderer (optional)
│   │       ├── openxr_context.h
│   │       ├── openxr_session.h
│   │       ├── openxr_renderer.h
│   │       └── ...
│   ├── application.h      # Main application class
│   ├── application.cpp
│   └── CMakeLists.txt
├── samples/               # Sample applications
│   ├── sample_main.cpp           # Desktop sample
│   ├── sample_vr_main.cpp        # VR sample (with OpenXR)
│   └── CMakeLists.txt
├── CMakeLists.txt
├── README.md
└── GRAPHICS_LAYER.md      # Detailed graphics and OpenXR documentation
```

## Architecture

### Shared Vulkan Context (Option B)

The graphics layer implements a **shared VulkanContext** architecture enabling simultaneous Desktop + VR rendering:

```
Application (owns VulkanContext lifecycle)
    │
    ├─ Shared VulkanContext
    │   ├─ VkInstance (single)
    │   ├─ VkDevice (single)
    │   └─ Graphics Queue (single)
    │
    ├─ VulkanRenderer (Desktop)
    │   └─ Desktop swapchain
    │
    └─ OpenXRRenderer (VR - optional)
        └─ OpenXR swapchains (stereo)
```

**Benefits:**
- Single Vulkan initialization and device creation
- No resource duplication
- Shared graphics queue for both renderers
- Independent swapchains for desktop window and VR headset
- Optional VR support (can run desktop-only if needed)

### Core Layer (`src/core/`)
- **Logger**: Debug and info logging with timestamp support
- **Math**: Vector and matrix utilities using GLM

### Platform Layer (`src/platform/`)
- **Window Abstraction**: Cross-platform window management
- **SDL2 Implementation**: Window creation, event polling, input handling

### Graphics Layer (`src/graphics/`)
- **Abstract Renderer Interface**: Unified API for multiple backends
- **Vulkan Renderer**: GPU-accelerated desktop rendering with shared context
- **Vulkan Utils**: Common utilities (memory management, image/buffer creation)
- **OpenXR Renderer** (optional): Stereoscopic VR rendering using shared context

### Application Layer
- **Application Class**: Coordinates window, shared graphics context, and renderers
- **Sample Applications**: Desktop and VR examples

## OpenXR Integration

Ember supports cross-platform VR development via OpenXR. See [GRAPHICS_LAYER.md](GRAPHICS_LAYER.md) for detailed documentation.

### Current Status
- ✅ **Phase 1: Foundation** - COMPLETE
  - [x] Project structure and CMake integration
  - [x] Abstract renderer interface with VR methods
  - [x] Shared VulkanContext (enable_shared_from_this pattern)
  - [x] OpenXR context and session classes
  - [x] VulkanUtils common helpers module
  - [x] Sample applications (desktop + VR)
  - [x] Comprehensive documentation

- ⏳ **Phase 2: Swapchain Management** - TODO
  - [ ] Desktop swapchain creation
  - [ ] OpenXR swapchain creation and image management
  - [ ] Framebuffer allocation

- ⏳ **Phase 3: Frame Rendering** - TODO
  - [ ] Frame timing (xrWaitFrame)
  - [ ] View pose prediction
  - [ ] Projection matrix calculation
  - [ ] Frame submission (xrBeginFrame/xrEndFrame)

- ⏳ **Phase 4: Input System** - TODO
  - [ ] Controller action mapping
  - [ ] Hand tracking
  - [ ] Gesture recognition

### Supported VR Platforms (Target)
- Meta Quest 2/3/Pro
- HTC Vive series
- Valve Index
- Windows Mixed Reality
- PlayStation VR (via OpenXR wrapper)

## Usage Examples

### Desktop Rendering Only
```cpp
#include "application.h"

int main() {
    using namespace ember;
    
    Application app;
    
    platform::WindowConfig windowConfig{
        .title = "Ember Desktop App",
        .width = 1280,
        .height = 720,
        .vsync = true
    };
    
    graphics::RendererConfig rendererConfig{
        .width = 1280,
        .height = 720,
        .enableValidation = true,
        .enableVR = false  // Desktop only
    };
    
    if (!app.initialize(windowConfig, rendererConfig)) {
        return 1;
    }
    
    app.run();
    app.shutdown();
    return 0;
}
```

### Simultaneous Desktop + VR Rendering
```cpp
graphics::RendererConfig rendererConfig{
    .width = 1280,
    .height = 720,
    .enableValidation = true,
    .enableVR = true,           // Enable VR
    .xrAppName = "My VR App"
};

if (app.initialize(windowConfig, rendererConfig)) {
    // Both desktop and VR renderers initialized with shared context
    
    // Desktop rendering
    auto renderer = app.getRenderer();
    renderer->beginFrame();
    // Render scene
    renderer->endFrame();
    renderer->present();
    
    // VR rendering (to headset)
    auto vrRenderer = app.getVRRenderer();
    if (vrRenderer && vrRenderer->supportsVR()) {
        uint32_t viewCount = 2;
        graphics::EyeView views[2];
        if (vrRenderer->beginVRFrame(viewCount, views)) {
            // Render for left eye: views[0]
            // Render for right eye: views[1]
            vrRenderer->endVRFrame();
        }
    }
}
```

## Troubleshooting

### CMake Configuration Fails
- Ensure all dependencies are installed
- Check that Vulkan SDK is properly set up (run `vulkaninfo`)
- On Linux, you may need to set `LD_LIBRARY_PATH` for Vulkan

### OpenXR Not Found
- Install OpenXR SDK from [Khronos GitHub](https://github.com/KhronosGroup/OpenXR-SDK)
- Set `OpenXR_DIR` if CMake can't find it: `cmake .. -DOpenXR_DIR=/path/to/openxr`
- Use `-DEMBER_WITH_OPENXR=OFF` to disable VR support (still get desktop rendering)

### SDL2 Not Found
- On Linux: `sudo apt-get install libsdl2-dev`
- On macOS: `brew install sdl2`
- On Windows: Install SDL2 and set `SDL2_DIR` if needed

### Vulkan Device Not Found
- Check that your GPU supports Vulkan
- Update graphics drivers
- Run `vulkaninfo` to diagnose Vulkan setup

### VR Runtime Not Found
- Ensure a compatible VR runtime is installed
- Windows: Install SteamVR or Oculus software
- Check `XR_RUNTIME_JSON` environment variable
- Test with OpenXR runtime diagnostic tools

## Future Development

- ✅ Shared VulkanContext architecture
- ⏳ Swapchain and frame rendering
- ⏳ Input system (controllers, hand tracking)
- ⏳ Dynamic resolution scaling
- ⏳ Foveated rendering
- ⏳ Physics integration
- ⏳ Audio system
- ⏳ Scene serialization
- ⏳ Editor tools
- ⏳ Performance profiling hooks

## Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Commit your changes with clear messages
4. Push to the branch
5. Create a Pull Request

## License

[Specify your license here]

## Resources

- [Vulkan Guide](https://vulkan-tutorial.com/)
- [OpenXR Specification](https://www.khronos.org/openxr/)
- [OpenXR SDK](https://github.com/KhronosGroup/OpenXR-SDK)
- [OpenXR Best Practices](https://developer.oculus.com/resources/)
- [VR Development Best Practices](https://developer.oculus.com/design/)
