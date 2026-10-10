#pragma once

#include <glm/glm.hpp>

#include "block/Neighbours.h"

namespace engine {
    class Block;
    struct LayerData;

    struct MeshEmitContext {
        const Block* block;
        glm::ivec3 posInChunk;
        glm::ivec3 worldPos;
        Neighbours neighbours;
        LayerData* storage;
    };
}  // namespace engine