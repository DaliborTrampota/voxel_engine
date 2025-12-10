#pragma once

#include <glm/glm.hpp>
#include "level/World.h"

namespace engine {

    /// @brief Returns the chunk coordinates of the given position and modifies the position to be relative to the chunk.
    /// @param pos Position in world space
    /// @return Coordinates of the chunk containing the position and the position of the block within the chunk is stored into pos
    ChunkID extractChunkCoords(glm::vec3& pos);
    ChunkID extractChunkCoords(glm::ivec3& pos);

    glm::ivec3 toChunkCoords(const ChunkID& chunkID, const glm::vec3& pos);

    ChunkID getChunkID(const glm::vec3& pos);

    /// @brief Returns the position floored, so position of the block in world space.
    /// @return The position floored, so position of the block in world space.
    glm::ivec3 floorToInt(const glm::vec3& vec);

    inline int floorDiv(int a, int b) {
        return (a >= 0) ? (a / b) : ((a - b + 1) / b);
    }

    /// @brief Returns positions of blocks that intersect the line between start and end.
    std::vector<glm::ivec3> traceLine(glm::vec3 start, glm::vec3 end);


    /// @brief Returns the direction the block should face when placed.
    /// @param lookDir The direction the player is looking
    /// @param rot The rotation mode of the block
    /// @param clickedFaceN The normal of the face the player clicked on
    /// @return The direction the block should face when placed
    glm::vec3 getFacingDirection(glm::vec3 lookDir, RotationMode rot, glm::ivec3 clickedFaceN);

    // TODO use quaternions for rotations?
    glm::quat getRotationQuat(glm::vec3 lookDir, RotationMode rot, glm::ivec3 clickedFaceN);

    float getAngleToSide(Side side, glm::vec3 from, glm::vec3& axis);
    float getAngle(glm::vec3 from, glm::vec3 to, glm::vec3& axis);


}  // namespace engine
