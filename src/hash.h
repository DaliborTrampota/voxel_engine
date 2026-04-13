#pragma once

#include "level/ChunkID.h"
#include "random/Random.h"

#include <functional>

namespace std {
    template <>
    struct hash<glm::ivec3> {
        size_t operator()(const glm::ivec3& k) const {
            return engine::Random::hash3D(k.x, k.y, k.z);
        }
    };

    template <>
    struct hash<engine::ChunkID> {
        size_t operator()(const engine::ChunkID& k) const {
            return engine::Random::hash3D(k.x, k.y, k.z);
        }
    };

}  // namespace std
