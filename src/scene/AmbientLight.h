#pragma once

#include <glm/vec3.hpp>

namespace engine {
    struct AmbientLight {
        glm::vec3 skyAmbient{0.3f};
        glm::vec3 groundAmbient{0.3f};
        glm::vec3 up{0.0f, 1.0f, 0.0f};
    };
}  // namespace engine