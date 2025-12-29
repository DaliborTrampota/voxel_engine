#include "Rotation.h"

#include "block/BlockState.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/vector_angle.hpp>

namespace engine {

    glm::vec3 getFacingDirection(glm::vec3 lookDir, RotationMode rot, glm::ivec3 clickedFaceN) {
        lookDir = -lookDir;  // make the look direction face the player from the block's perspective
        switch (rot) {
            case RotationMode::None: return {0, 0, 0};
            case RotationMode::AxisY: return glm::normalize(glm::vec3{lookDir.x, 0, lookDir.z});
            case RotationMode::AxisXYZ: return lookDir;
            case RotationMode::AxisYSnap: {
                float zDot = glm::abs(glm::dot(lookDir, NORTH));
                float xDot = glm::abs(glm::dot(lookDir, EAST));
                if (xDot > zDot) {
                    return lookDir.x > 0 ? EAST : -EAST;
                } else {
                    return lookDir.z > 0 ? NORTH : -NORTH;
                }
            }
            case RotationMode::AxisXYZSnap:
            case RotationMode::AxisAngleXYZSnap: {
                float northDot = glm::abs(glm::dot(lookDir, NORTH));
                float upDot = glm::abs(glm::dot(lookDir, UP));
                float eastDot = glm::abs(glm::dot(lookDir, EAST));

                //TODO change to glm::vec and "round" to closest axis
                glm::vec3 facingDir = clickedFaceN;
                static constexpr float DOT_30 = 0.866f;
                static constexpr float DOT_45 = 0.707f;

                glm::vec3 resultDirection;

                // Determine dominant axis by comparing dot products
                if (northDot > upDot && northDot > eastDot) {
                    // Z-axis (NORTH/SOUTH) is dominant
                    if (northDot <= DOT_30)
                        resultDirection = lookDir.z > 0 ? NORTH : -NORTH;
                    else
                        resultDirection = facingDir;
                } else if (upDot > northDot && upDot > eastDot) {
                    // Y-axis (UP/DOWN) is dominant
                    if (rot == RotationMode::AxisAngleXYZSnap) {
                        // Calculate the angle in radians between NORTH and lookDir with Y component set to zero
                        glm::vec3 lookDirXZ = glm::normalize(glm::vec3(lookDir.x, 0.0f, lookDir.z));
                        float angle =
                            glm::acos(glm::clamp(glm::dot(NORTH, lookDirXZ), -1.0f, 1.0f));

                        // TODO return pair?
                    }
                    if (upDot <= DOT_30)
                        resultDirection = lookDir.y > 0 ? UP : -UP;
                    else
                        resultDirection = facingDir;
                } else {
                    // X-axis (EAST/WEST) is dominant
                    if (eastDot <= DOT_30)
                        resultDirection = lookDir.x > 0 ? EAST : -EAST;
                    else
                        resultDirection = facingDir;
                }

                if (rot == RotationMode::AxisAlign) {
                    return glm::abs(resultDirection);
                } else {
                    return resultDirection;
                }
            }
            case RotationMode::AxisAlign: {
                float northDot = glm::abs(glm::dot(lookDir, NORTH));
                float upDot = glm::abs(glm::dot(lookDir, UP));
                float eastDot = glm::abs(glm::dot(lookDir, EAST));

                //TODO change to glm::vec and "round" to closest axis
                glm::vec3 facingDir = clickedFaceN;
                static constexpr float DOT_15 = 0.966f;
                static constexpr float DOT_30 = 0.866f;
                static constexpr float DOT_45 = 0.707f;

                glm::vec3 resultDirection;

                // Determine dominant axis by comparing dot products
                if (northDot > upDot && northDot > eastDot) {
                    if (northDot >= DOT_30)
                        resultDirection = NORTH;
                    else
                        resultDirection = facingDir;
                } else if (upDot > northDot && upDot > eastDot) {
                    if (upDot >= DOT_30)
                        resultDirection = UP;
                    else
                        resultDirection = facingDir;
                } else {
                    if (eastDot >= DOT_30)
                        resultDirection = EAST;
                    else
                        resultDirection = facingDir;
                }
                return resultDirection;
            }
        }
    }

    // glm::quat getRotationQuat(glm::vec3 lookDir, RotationMode rot, glm::ivec3 clickedFaceN) {


    // }

    float getAngleToSide(Side side, glm::vec3 from, glm::vec3& axis) {
        glm::vec3 sideDir = sideDirection(side);
        axis = glm::normalize(glm::cross(sideDir, from));
        if (glm::all(glm::isnan(axis))) {
            axis = glm::normalize(glm::vec3(from.y, from.z, from.x));
        }
        return glm::orientedAngle(from, sideDir, axis);
    }

    float getAngleFromSide(Side side, glm::vec3 to, glm::vec3& axis) {
        glm::vec3 sideDir = sideDirection(side);
        axis = glm::normalize(glm::cross(to, sideDir));
        if (glm::all(glm::isnan(axis))) {
            axis = glm::normalize(glm::vec3(to.y, to.z, to.x));
        }
        return glm::orientedAngle(sideDir, to, axis);
    }

    float getAngle(glm::vec3 from, glm::vec3 to, glm::vec3& axis) {
        axis = glm::normalize(glm::cross(to, from));
        if (glm::all(glm::isnan(axis))) {
            axis = glm::normalize(glm::vec3(from.y, from.z, from.x));
        }
        return glm::orientedAngle(from, to, axis);
    }

    //TODO better name
    glm::mat4 transformMatrix(glm::vec3 worldPos, glm::vec3 axis, float angle) {
        return glm::translate(glm::mat4(1.0f), worldPos) *
               glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f)) *
               glm::rotate(glm::mat4(1.0f), angle, axis) *
               glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.5f, -0.5f));
    }

    void calculateRotationFromState(
        const BlockState* state, const Block* block, float& angle, glm::vec3& axis
    ) {
        if (state && block->rotationMode() != RotationMode::None) {
            Side baseSide = block->facingUp() ? Side::Up : Side::North;
            glm::vec3 baseSideDir = sideDirection(baseSide);
            const glm::vec3& facing = state->facing();

            angle = getAngle(baseSideDir, facing, axis);
            // geoState.axis = glm::vec3(0, 1, 0);
            // if (glm::any(glm::isnan(geoState.axis))) {
            // } else {
            // }
        } else {
            angle = 0.0f;
            axis = glm::vec3(0, 1, 0);
        }
    }
}  // namespace engine