#pragma once

#include <glm/glm.hpp>

#include "block/Neighbours.h"

namespace engine {
    class Block;
    class BlockState;
    struct LayerData;

    struct MeshEmitContext {
        const Block* block;
        const BlockState* state;  // null for stateless blocks
        glm::ivec3 posInChunk;
        glm::ivec3 worldPos;
        Neighbours neighbours;
        LayerData* storage;
    };
}  // namespace engine