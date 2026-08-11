# Ember Engine

A modern C++ graphics engine with Vulkan support and layered architecture.

## Features

- **Vulkan Rendering**: Modern GPU-accelerated graphics using Vulkan API
- **SDL2 Window Management**: Cross-platform window and input handling
- **Layered Architecture**: Clean separation between core, platform, and graphics layers
- **Sample Application**: Ready-to-build example demonstrating engine capabilities

## Dependencies

- **CMake** 3.20 or higher
- **C++17** compatible compiler (GCC, Clang, MSVC)
- **Vulkan SDK** (1.2 or higher)
- **SDL2** (2.0.9 or higher)
- **GLM** (0.9.9.8 or higher)

### Installation

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install cmake build-essential vulkan-tools libvulkan-dev libvulkan1 libsdl2-dev libglm-dev
```

#### macOS
```bash
brew install cmake vulkan-headers vulkan-loader sdl2 glm
```

#### Windows
Download and install Vulkan SDK, SDL2, and GLM from their respective websites.

## Building

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Debug
./samples/ember_sample
```

## Architecture

- **Core Layer** (`src/core/`): Logger, math utilities
- **Platform Layer** (`src/platform/`): SDL2 window management
- **Graphics Layer** (`src/graphics/`): Vulkan renderer
- **Application Layer**: Main application class and sample

## Future Development

- Complete Vulkan rendering pipeline
- Input system
- Physics integration
- Audio system
- Scene serialization
