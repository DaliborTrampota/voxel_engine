#include "Neighbours.h"

#include <glm/gtc/matrix_transform.hpp>
#include "utility/Rotation.h"


using namespace engine;


BlockID Neighbours::operator[](Side direction) const {
    if (direction == Side::Invalid)
        return InvalidBlockID;
    return sides[static_cast<size_t>(direction)];
}

glm::vec3 Neighbours::getFacing(Side direction) const {
    if (direction == Side::Invalid)
        return glm::vec3(0.0f);
    return facings[static_cast<size_t>(direction)];
}

void Neighbours::rotate(Side from, Side to) {
    if (from == to)
        return;

    Neighbours original = *this;

    glm::vec3 axis;
    float angle = getAngle(sideDirection(from), sideDirection(to), axis);

    glm::mat4 rotateMat = glm::rotate(glm::mat4(1.0f), angle, axis);
    auto rotateSide = [&](Side side) -> BlockID {
        glm::vec3 dir = sideDirection(side);
        glm::vec3 rotated = glm::vec3(rotateMat * glm::vec4(dir, 0.0f));

        return original[getSide(glm::round(rotated))];
    };

    auto rotateFacing = [&](Side side) -> glm::vec3 {
        // Get rotated side and rotate that facing dir
        glm::vec3 dir = sideDirection(side);
        glm::vec3 fromRotation = glm::vec3(rotateMat * glm::vec4(dir, 0.0f));

        glm::vec3 facing = original.getFacing(getSide(glm::round(fromRotation)));
        if (glm::length(facing) < 0.01f) {
            return facing;  // No rotation for invalid/zero facings
        }
        glm::vec3 rotated = glm::vec3(rotateMat * glm::vec4(facing, 0.0f));
        // TODO/NOTE: Should check here for rotation mode of the block and if its AxisAlign and if so, glm::abs(rotated)
        return glm::normalize(rotated);
    };

    for (auto side : IterateSides) {
        sides[static_cast<size_t>(side)] = rotateSide(side);
        facings[static_cast<size_t>(side)] = rotateFacing(side);
    }
}