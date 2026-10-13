#pragma once

#include <cstdint>

namespace ember::core {

/// @brief Blend mode for fixed-function alpha blending state.
enum class BlendMode : uint8_t { Opaque, AlphaBlend, Additive };

/// @brief Face culling mode for rasterization.
enum class CullMode : uint8_t { None, Front, Back };

}  // namespace ember::core