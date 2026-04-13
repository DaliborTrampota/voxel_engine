#pragma once

#include <glm/glm.hpp>
#include <string>

namespace engine {

    struct ChunkID : glm::ivec3 {
        ChunkID(int x, int y, int z) : glm::ivec3(x, y, z) {}
        ChunkID(const glm::ivec3& vec) : glm::ivec3(vec) {}

        bool operator==(const ChunkID& other) const;
        std::string toString() const;
    };
};  // namespace engine