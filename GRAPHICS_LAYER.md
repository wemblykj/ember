# Ember Graphics Layer Documentation

The graphics layer provides an abstract rendering interface supporting multiple backends for desktop and VR rendering.

## Architecture Overview

```
graphics/
├── renderer.h              # Abstract renderer interface
├── renderer_config.h       # Configuration structures
├── vulkan/                 # Vulkan desktop rendering
│   ├── vulkan_renderer.h
│   ├── vulkan_context.h
│   └── vulkan_renderer.cpp
└── openxr/                 # OpenXR VR rendering (optional)
    ├── openxr_renderer.h
    ├── openxr_context.h
    ├── openxr_session.h
    ├── openxr_renderer.cpp
    ├── openxr_context.cpp
    └── openxr_session.cpp
```

## Available Backends

### Vulkan (Primary - Always Enabled)
- Modern GPU-accelerated rendering
- Cross-platform support (Windows, Linux, macOS)
- Comprehensive validation layers
- High performance graphics pipeline
- **Status**: Stable
- **Path**: `src/graphics/vulkan/`

### OpenXR (Optional - VR Support)
- Cross-platform VR rendering via OpenXR standard
- Support for Meta Quest, HTC Vive, Valve Index, and other devices
- Stereoscopic rendering for VR headsets
- Hand tracking and controller input (planned)
- **Status**: In Development
- **Path**: `src/graphics/openxr/`
- **Enable with**: `-DEMBER_WITH_OPENXR=ON`

## Renderer Interface

The abstract `Renderer` class provides a unified interface for both rendering modes.

### Desktop Rendering Path
```cpp
void renderDesktop(Renderer* renderer) {
    renderer->beginFrame();
    
    // Your rendering code here
    
    renderer->endFrame();
    renderer->present();
}
```

### VR Rendering Path (OpenXR)
```cpp
void renderVR(Renderer* renderer) {
    uint32_t viewCount = 2;
    graphics::EyeView views[2];
    
    if (renderer->beginVRFrame(viewCount, views)) {
        // Render for left eye (views[0])
        // views[0].projectionMatrix   - Projection matrix for left eye
        // views[0].viewMatrix         - View matrix for left eye
        
        // Render for right eye (views[1])
        // views[1].projectionMatrix   - Projection matrix for right eye
        // views[1].viewMatrix         - View matrix for right eye
        
        renderer->endVRFrame();
    }
}
```

## Configuration

### RendererConfig Structure
```cpp
struct RendererConfig {
    uint32_t width = 1280;           // Display width
    uint32_t height = 720;           // Display height
    bool vsync = true;               // Enable vertical sync
    bool enableValidation = false;   // Enable debug validation
    
    // VR-specific
    bool enableVR = false;           // Enable VR mode
    std::string xrAppName = "...";   // OpenXR application name
};
```

### Building with OpenXR

```bash
# Configure with OpenXR enabled
cmake .. -DEMBER_WITH_OPENXR=ON

# Build with verbose output
cmake --build . --config Debug --verbose

# The OpenXR code is conditionally compiled with EMBER_OPENXR_ENABLED
```

## OpenXR Implementation Details

### Components

**OpenXRContext** - Low-level OpenXR API management
- Creates and manages XrInstance
- Enumerates system properties
- Binds graphics API (Vulkan)
- Retrieves system ID for headset properties

**OpenXRSession** - VR session lifecycle management
- Creates XrSession with graphics bindings
- Manages swapchains for left/right eyes
- Handles frame timing synchronization
- Calculates view projections from poses
- Submits rendered frames to runtime

**OpenXRRenderer** - High-level VR interface
- Implements the Renderer interface
- Coordinates OpenXRContext and OpenXRSession
- Provides EyeView data for stereo rendering
- Manages session state transitions

### Implementation Roadmap

#### Phase 1: Foundation (Current ✅)
- [x] Project structure and CMake integration
- [x] Abstract renderer interface with VR methods
- [x] OpenXR header files and class definitions
- [x] Stub implementations for compilation
- [ ] XrInstance creation and enumeration

#### Phase 2: Core Session ⏳
- [ ] Graphics binding (Vulkan + OpenXR)
- [ ] XrSession initialization
- [ ] Reference space setup
- [ ] Swapchain creation for eyes
- [ ] Swapchain image acquisition

#### Phase 3: Frame Loop ⏳
- [ ] Frame timing (xrWaitFrame)
- [ ] View pose prediction
- [ ] Projection matrix calculation
- [ ] Frame submission (xrBeginFrame/xrEndFrame)
- [ ] Layer composition

#### Phase 4: Input System ⏳
- [ ] Action set creation
- [ ] Controller action mapping
- [ ] Hand tracking (if supported)
- [ ] Gesture recognition

#### Phase 5: Performance ⏳
- [ ] Dynamic resolution scaling
- [ ] Foveated rendering
- [ ] GPU profiling hooks
- [ ] Frame time analysis

### Key Classes and Methods

```cpp
// OpenXRContext
class OpenXRContext {
    bool initialize(const std::string& appName);
    void shutdown();
    void* getInstance() const;
    uint64_t getSystemId() const;
};

// OpenXRSession
class OpenXRSession {
    bool initialize();
    void shutdown();
    bool waitFrame();          // Synchronize with frame timing
    bool beginFrame();         // Start frame submission
    bool endFrame();           // Submit frame to runtime
    uint32_t getViewCount();   // Usually 2 for stereo
    void* getViews();          // XrView array pointer
};

// OpenXRRenderer (derives from Renderer)
class OpenXRRenderer : public Renderer {
    bool initialize() override;
    void shutdown() override;
    bool beginVRFrame(uint32_t& viewCount, EyeView* views) override;
    void endVRFrame() override;
    bool supportsVR() const override;  // Returns true
    bool isVRActive() const override;
};
```

## Performance Considerations for VR

### Frame Rate Requirements
- **Target**: 90 FPS (Meta Quest 3, HTC Vive Pro)
- **Minimum**: 72 FPS (Legacy devices)
- **Failure threshold**: Below 72 FPS causes motion sickness

### Latency Budget
- **Motion-to-photon**: < 20ms (ideal)
- **Frame time**: ~11ms per frame at 90 FPS
- Critical for comfortable VR experiences

### Optimization Strategies
1. **Frustum Culling**: Two separate frustums for each eye
2. **Level of Detail**: Aggressive LOD culling for VR
3. **Texture Compression**: Use BC4/BC5 formats
4. **Instancing**: Reduce draw calls with GPU instancing
5. **Resolution**: Start with 1024x1024 per eye, adjust for performance

### Profiling Tools
- NVIDIA FrameView
- AMD Radeon GPU Profiler
- RenderDoc for frame analysis
- OpenXR debug utilities

## Extending the Graphics Layer

To add a new backend (e.g., Direct3D 12):

1. Create directory: `src/graphics/directx12/`
2. Create headers:
   - `directx12_renderer.h`
   - `directx12_context.h`
3. Implement the `Renderer` interface
4. Add CMake option and sources:
   ```cmake
   option(EMBER_WITH_DIRECTX12 "Enable Direct3D 12 backend" OFF)
   if(EMBER_WITH_DIRECTX12)
       target_sources(ember_graphics PRIVATE directx12/...)
       target_compile_definitions(ember_graphics PUBLIC EMBER_DIRECTX12_ENABLED)
   endif()
   ```
5. Create factory function: `createDirect3D12Renderer()`

## Testing OpenXR

### Development Without Hardware
- Use [OpenXR Simulator](https://github.com/microsoft/OpenXR-SDK/tree/master/src/common/windows) (Windows)
- Use Monado on Linux
- Mock OpenXR for unit testing

### With Real Hardware
Supported devices (via OpenXR):
- Meta Quest 2/3/Pro
- Meta Rift S
- HTC Vive (full suite)
- Valve Index
- Windows Mixed Reality
- PlayStation VR (via OpenXR wrapper on PC)
- Pico devices

### Testing Checklist
- [ ] Render scene to both eyes
- [ ] Verify head tracking
- [ ] Check frame rate at 90 FPS
- [ ] Test controller input
- [ ] Validate motion-to-photon latency
- [ ] Test scene scale and movement
- [ ] Verify comfort settings

## Troubleshooting OpenXR

### "OpenXR runtime not found"
- Ensure a compatible VR runtime is installed
- Windows: Install SteamVR or Oculus software
- Check `XR_RUNTIME_JSON` environment variable

### Frame rate dropping below 90 FPS
- Profile GPU with RenderDoc
- Check view resolution
- Reduce draw calls
- Enable MSAA instead of post-process AA

### Head tracking not working
- Verify OpenXR runtime can track device
- Check calibration
- Test with OpenXR runtime's diagnostic tools

### Controllers not responding
- Verify action set bindings
- Check controller battery levels
- Test with manufacturer's app first

## References

- [OpenXR Specification](https://www.khronos.org/openxr/)
- [OpenXR SDK & Docs](https://github.com/KhronosGroup/OpenXR-SDK)
- [Khronos OpenXR Resources](https://www.khronos.org/openxr/resources/)
- [VR Development Best Practices](https://developer.oculus.com/design/)
- [Vulkan Guide](https://vulkan-tutorial.com/)
- [OpenXR Best Practices](https://developer.oculus.com/resources/)
