#pragma once

#include <cstdint>

namespace ember::graphics {

enum class GraphicsAPI : uint8_t {
    Default = 0,
    Vulkan,
    OpenGL,
    DirectX12,
    Metal
};

}
