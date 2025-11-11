#pragma once

#include <cstdint>
// OpenGL Defaults
#define UP glm::vec3{0, 1, 0}
#define FORWARD glm::vec3{0, 0, -1}


namespace engine {
    using TexID = unsigned int;
    using BlockID = uint32_t;
    using Layer = uint8_t;

    // Render layer constants
    namespace Layers {
        constexpr Layer ANY = 0;
        constexpr Layer OPAQUE = 1;
        constexpr Layer TRANSPARENT = 2;
    }  // namespace Layers
}  // namespace engine