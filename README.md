# Ember Engine

A modern C++ graphics engine with Vulkan support and optional OpenXR VR capabilities.

## Features

- **Vulkan Rendering**: Modern GPU-accelerated graphics using Vulkan API
- **OpenXR VR Support**: Cross-platform VR rendering (optional, in development)
- **SDL2 Window Management**: Cross-platform window and input handling
- **Layered Architecture**: Clean separation between core, platform, and graphics layers
- **Sample Application**: Ready-to-build example demonstrating engine capabilities

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

## Building

### Desktop Build (Vulkan only)

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Debug
./samples/ember_sample
```

### With OpenXR Support

```bash
mkdir build && cd build
cmake .. -DEMBER_WITH_OPENXR=ON
cmake --build . --config Debug
./samples/ember_sample
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
│   │   └── openxr/        # OpenXR VR renderer (optional)
│   ├── application.h      # Main application class
│   ├── application.cpp
│   └── CMakeLists.txt
├── samples/               # Sample applications
│   ├── sample_main.cpp
│   ├── sample_scene.h
│   └── CMakeLists.txt
├── CMakeLists.txt
├── README.md
└── GRAPHICS_LAYER.md      # Detailed graphics and OpenXR documentation
```

## Architecture

### Core Layer (`src/core/`)
- **Logger**: Debug and info logging with timestamp support
- **Math**: Vector and matrix utilities using GLM

### Platform Layer (`src/platform/`)
- **Window Abstraction**: Cross-platform window management
- **SDL2 Implementation**: Window creation, event polling, input handling

### Graphics Layer (`src/graphics/`)
- **Abstract Renderer Interface**: Unified API for multiple backends
- **Vulkan Renderer**: GPU-accelerated desktop rendering
- **OpenXR Renderer** (optional): Stereoscopic VR rendering

### Application Layer
- **Application Class**: Coordinates window, renderer, and lifecycle
- **Sample Application**: Demonstrates engine usage

## OpenXR Integration (In Development)

Ember is being extended with comprehensive OpenXR support for cross-platform VR development. See [GRAPHICS_LAYER.md](GRAPHICS_LAYER.md) for detailed OpenXR documentation.

### Current Status
- ✅ Project structure and CMake integration
- ✅ Abstract renderer interface with VR methods
- ✅ OpenXR context and session classes
- ✅ Header files and stub implementations
- ⏳ Swapchain management (TODO)
- ⏳ Full frame rendering loop (TODO)
- ⏳ Input system integration (TODO)

### Supported VR Platforms (Target)
- Meta Quest 2/3/Pro
- HTC Vive series
- Valve Index
- Windows Mixed Reality
- PlayStation VR (via OpenXR wrapper)

## Usage Examples

### Desktop Rendering
```cpp
#include "application.h"

int main() {
    using namespace ember;
    
    Application app;
    
    platform::WindowConfig windowConfig{
        .title = "Ember Application",
        .width = 1280,
        .height = 720,
        .vsync = true
    };
    
    graphics::RendererConfig rendererConfig{
        .width = 1280,
        .height = 720,
        .enableValidation = true
    };
    
    if (!app.initialize(windowConfig, rendererConfig)) {
        return 1;
    }
    
    app.run();
    app.shutdown();
    return 0;
}
```

### VR Rendering (With OpenXR)
```cpp
graphics::RendererConfig rendererConfig{
    .enableVR = true,
    .xrAppName = "My VR App"
};

if (app.initialize(windowConfig, rendererConfig)) {
    // VR rendering in main loop
    auto renderer = app.getRenderer();
    if (renderer->supportsVR()) {
        uint32_t viewCount = 2;
        graphics::EyeView views[2];
        if (renderer->beginVRFrame(viewCount, views)) {
            // Render for left eye: views[0]
            // Render for right eye: views[1]
            renderer->endVRFrame();
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
- Use `-DEMBER_WITH_OPENXR=OFF` to disable VR support

### SDL2 Not Found
- On Linux: `sudo apt-get install libsdl2-dev`
- On macOS: `brew install sdl2`
- On Windows: Install SDL2 and set `SDL2_DIR` if needed

### Vulkan Device Not Found
- Check that your GPU supports Vulkan
- Update graphics drivers
- Run `vulkaninfo` to diagnose Vulkan setup

## Future Development

- ✅ OpenXR foundation (in progress)
- ⏳ Swapchain and frame rendering
- ⏳ Input system (controllers, hand tracking)
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
- [VR Development Best Practices](https://developer.oculus.com/design/)
