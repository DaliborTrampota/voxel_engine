#pragma once

#include <memory>

#include "../input/InputTypes.h"
#include "Face.h"


namespace engine {

    class World;

    struct InteractContext {
        std::weak_ptr<World> world;
        glm::ivec3 position;
        glm::vec3 normal;
        FaceTag face;
        GLFWKey button;
        void* data;
        // TODO modifiers? (shift, ctrl, etc)
    };
}  // namespace engine