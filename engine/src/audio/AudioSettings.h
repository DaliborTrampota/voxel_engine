#pragma once

#include <glm/glm.hpp>

namespace engine {

    struct AudioSettings {
        float volume = 1.0f;
        bool loop = false;
        bool background = false;
        bool directional = false;
        struct {
            glm::vec3 position;
            glm::vec3 acceleration = {0.0f, 0.0f, 0.0f};
        } directionalData;
    };

}  // namespace engine