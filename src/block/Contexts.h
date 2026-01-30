#pragma once

#include "../input/InputTypes.h"
#include "../utility/Algorithms.h"
#include "Face.h"


namespace engine {

    class Chunk;
    class BlockState;

    struct InteractContext {
        Chunk* chunk;
        /// @brief The position of the block in the chunk.
        glm::ivec3 position;
        glm::vec3 normal;
        FaceTag face;
        GLFWKey button;
        bool shiftPressed = false;
        bool ctrlPressed = false;
        bool altPressed = false;
        void* data = nullptr;
    };

    struct BlockSetContext {
        Chunk* chunk;
        /// @brief The position of the block in the chunk.
        glm::ivec3 position;
        BlockState* state;
        void* data = nullptr;
    };
}  // namespace engine