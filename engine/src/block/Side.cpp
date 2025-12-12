#include "Side.h"

#include "Globals.h"

using namespace engine;

glm::vec3 engine::sideDirection(Side side) {
    switch (side) {
        case Side::North: return NORTH;
        case Side::South: return -NORTH;
        case Side::East: return EAST;
        case Side::West: return -EAST;
        case Side::Up: return UP;
        case Side::Down: return -UP;
        case Side::Invalid: return glm::vec3(0, 0, 0);
    }
}

Side engine::getSide(glm::ivec3 direction) {
    if (direction == INORTH)
        return Side::North;
    if (direction == -INORTH)
        return Side::South;
    if (direction == IEAST)
        return Side::East;
    if (direction == -IEAST)
        return Side::West;
    if (direction == IUP)
        return Side::Up;
    if (direction == -IUP)
        return Side::Down;
    return Side::Invalid;
}