#pragma once

#include <cstdint>
#include <iosfwd>
#include <limits>


// OpenGL Defaults
#define UP glm::vec3{0, 1, 0}
#define NORTH glm::vec3{0, 0, 1}
#define EAST glm::vec3{1, 0, 0}

#define IUP glm::ivec3{0, 1, 0}
#define INORTH glm::ivec3{0, 0, 1}
#define IEAST glm::ivec3{1, 0, 0}


namespace engine {
    using TexID = unsigned int;
    using BlockID = uint32_t;
    using Layer = uint8_t;

    struct ChunkID;

    constexpr BlockID InvalidBlockID = std::numeric_limits<BlockID>::max();

    // Render layer constants
    namespace Layers {
        constexpr Layer Any = 0;
        constexpr Layer Opaque = 1;
        constexpr Layer Transparent = 2;
    }  // namespace Layers


}  // namespace engine

std::ostream& operator<<(std::ostream& os, const engine::ChunkID& chID);