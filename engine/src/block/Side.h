#pragma once

#include <glm/glm.hpp>

namespace engine {
    enum class Side {
        North,
        West,
        South,
        East,
        Up,
        Down,
        Invalid
    };

    // Here was a TODO with a funny comment I want to preserve :)
    // place elsewhere? connect somehow with Side? I drank over 20 teas from all over the world close to cofein overdose
    glm::ivec3 sideDirection(Side side);
    Side getSide(glm::ivec3 direction);
}  // namespace engine