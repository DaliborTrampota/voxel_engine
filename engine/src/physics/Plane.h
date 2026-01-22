#pragma once

#include <glm/glm.hpp>

namespace engine {
    struct Plane {
        Plane(glm::vec3 normal, float distance) {
            float len = glm::length(normal);
            this->normal = normal / len;
            this->distance = distance / len;
        }
        glm::vec3 normal{0.0f, 1.0f, 0.0f};
        float distance{0.0f};
    };
}  // namespace engine