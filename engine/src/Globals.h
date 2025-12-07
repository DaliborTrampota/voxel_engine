#pragma once

#include <cstdint>
#include <limits>

// OpenGL Defaults
#define UP glm::vec3{0, 1, 0}
#define FORWARD glm::vec3{0, 0, -1}


namespace engine {
    using TexID = unsigned int;
    using BlockID = uint32_t;
    using Layer = uint8_t;

    constexpr BlockID InvalidBlockID = std::numeric_limits<BlockID>::max();

    // Render layer constants
    namespace Layers {
        constexpr Layer Any = 0;
        constexpr Layer Opaque = 1;
        constexpr Layer Transparent = 2;
    }  // namespace Layers
}  // namespace engine