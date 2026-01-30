#pragma once

#include <soloud.h>
#include <glm/glm.hpp>


namespace engine {

    struct AudioSettings {
        float volume = 1.0f;
        bool loop = false;
        bool background = false;
        bool directional = false;
        struct {
            glm::vec3 position;
            glm::vec3 velocity = {0.0f, 0.0f, 0.0f};
            float minDistance = 1.0f;     // Distance at which sound is at full volume
            float maxDistance = 1000.0f;  // Distance at which sound is inaudible
            float rolloff = 1.0f;         // How quickly sound attenuates
            unsigned int attenuationModel = SoLoud::AudioSource::INVERSE_DISTANCE;
        } directionalData;
    };

}  // namespace engine