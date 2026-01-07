#pragma once

#include <glm/glm.hpp>

#include "block/Block.h"

namespace engine {
    class BlockState;

    /// @brief Returns the direction the block should face when placed.
    /// @param lookDir The direction the player is looking
    /// @param rot The rotation mode of the block
    /// @param clickedFaceN The normal of the face the player clicked on
    /// @return The direction the block should face when placed
    glm::vec3 getFacingDirection(
        glm::vec3 lookDir, RotationMode rot, glm::ivec3 clickedFaceN
    ) noexcept;

    // TODO use quaternions for rotations?
    //glm::quat getRotationQuat(glm::vec3 lookDir, RotationMode rot, glm::ivec3 clickedFaceN);

    float getAngleToSide(Side side, glm::vec3 from, glm::vec3& axis) noexcept;
    float getAngleFromSide(Side side, glm::vec3 to, glm::vec3& axis) noexcept;
    float getAngle(glm::vec3 from, glm::vec3 to, glm::vec3& axis) noexcept;

    /// @brief Rotate around block center and translate to worldPos
    glm::mat4 transformMatrix(glm::vec3 t, glm::vec3 axis, float angle) noexcept;

    void calculateRotationFromState(
        const BlockState* state, const Block* block, float& angle, glm::vec3& axis
    ) noexcept;

    glm::ivec3 closestAxis(glm::vec3 vec) noexcept;

}  // namespace engine