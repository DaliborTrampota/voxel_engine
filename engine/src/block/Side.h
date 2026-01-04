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

    constexpr Side IterateSides[6] = {
        Side::North, Side::South, Side::East, Side::West, Side::Up, Side::Down
    };

    constexpr Side IterateXZSides[4] = {Side::North, Side::South, Side::East, Side::West};

    constexpr Side IterateYSides[2] = {Side::Up, Side::Down};
}  // namespace engine