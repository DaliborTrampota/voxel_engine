#include "CoordUtils.h"

namespace engine {

    ChunkID extractChunkCoords(glm::vec3& pos) {
        ChunkID chunkCoords =
            ChunkID(static_cast<glm::ivec3>(glm::floor(pos / glm::vec3(Chunk::Dims))));
        pos -= chunkCoords * Chunk::Dims;
        return chunkCoords;
    }

    ChunkID extractChunkCoords(glm::ivec3& pos) {
        ChunkID chunkCoords = ChunkID(
            floorDiv(pos.x, Chunk::Dims.x),
            floorDiv(pos.y, Chunk::Dims.y),
            floorDiv(pos.z, Chunk::Dims.z)
        );
        pos -= chunkCoords * Chunk::Dims;
        return chunkCoords;
    }

    glm::ivec3 toChunkCoords(const ChunkID& chunkID, const glm::vec3& pos) {
        return static_cast<glm::ivec3>(glm::floor(pos)) - chunkID * Chunk::Dims;
    }

    ChunkID getChunkID(const glm::vec3& pos) {
        return ChunkID(static_cast<glm::ivec3>(glm::floor(pos / glm::vec3(Chunk::Dims))));
    }

    glm::ivec3 floorToInt(const glm::vec3& vec) {
        return static_cast<glm::ivec3>(vec);
    }

    std::vector<glm::ivec3> traceLine(glm::vec3 start, glm::vec3 end) {
        std::vector<glm::ivec3> visited;

        glm::vec3 dir = glm::normalize(end - start);
        glm::ivec3 currentBlock = glm::floor(start);

        glm::vec3 tMax;
        glm::vec3 tDelta;
        glm::ivec3 step;

        for (int i = 0; i < 3; ++i) {
            if (dir[i] > 0) {
                step[i] = 1;
                tMax[i] = ((currentBlock[i] + 1) - start[i]) / dir[i];
                tDelta[i] = 1.0f / dir[i];
            } else if (dir[i] < 0) {
                step[i] = -1;
                tMax[i] = (start[i] - currentBlock[i]) / -dir[i];
                tDelta[i] = 1.0f / -dir[i];
            } else {
                step[i] = 0;
                tMax[i] = std::numeric_limits<float>::infinity();
                tDelta[i] = std::numeric_limits<float>::infinity();
            }
        }

        float maxDist = glm::length(end - start);
        float travelled = 0.0f;

        while (travelled <= maxDist) {
            visited.push_back(currentBlock);

            if (tMax.x < tMax.y) {
                if (tMax.x < tMax.z) {
                    currentBlock.x += step.x;
                    travelled = tMax.x;
                    tMax.x += tDelta.x;
                } else {
                    currentBlock.z += step.z;
                    travelled = tMax.z;
                    tMax.z += tDelta.z;
                }
            } else {
                if (tMax.y < tMax.z) {
                    currentBlock.y += step.y;
                    travelled = tMax.y;
                    tMax.y += tDelta.y;
                } else {
                    currentBlock.z += step.z;
                    travelled = tMax.z;
                    tMax.z += tDelta.z;
                }
            }
        }

        return visited;
    }

    glm::vec3 getFacingDirection(glm::vec3 lookDir, RotationMode rot, glm::ivec3 clickedFaceN) {
        lookDir = -lookDir;  // make the look direction face the player from the block's perspective
        switch (rot) {
            case RotationMode::None: return {0, 0, 0};
            case RotationMode::AxisY: return glm::normalize(glm::vec3{lookDir.x, 0, lookDir.z});
            case RotationMode::AxisXYZ: return lookDir;
            case RotationMode::AxisYSnap: {
                float xDot = glm::dot(lookDir, NORTH);
                float zDot = glm::dot(lookDir, EAST);
                if (xDot > zDot) {
                    return lookDir.x > 0 ? NORTH : -NORTH;
                } else {
                    return lookDir.z > 0 ? EAST : -EAST;
                }
            }
            case RotationMode::AxisXYZSnap:
            case RotationMode::AxisAngleXYZSnap:
            case RotationMode::AxisAlign: {
                float northDot = glm::abs(glm::dot(lookDir, NORTH));
                float upDot = glm::abs(glm::dot(lookDir, UP));
                float eastDot = glm::abs(glm::dot(lookDir, EAST));

                //TODO change to glm::vec and "round" to closest axis
                glm::vec3 facingDir = clickedFaceN;
                static constexpr float DOT_30 = 0.707f;
                static constexpr float DOT_45 = 0.866f;

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
        }
    }

    // glm::quat getRotationQuat(glm::vec3 lookDir, RotationMode rot, glm::ivec3 clickedFaceN) {


    // }

    float getAngleToSide(Side side, glm::vec3 from, glm::vec3& axis) {
        glm::vec3 sideDir = sideDirection(side);
        axis = glm::abs(glm::normalize(glm::cross(from, sideDir)));
        return glm::orientedAngle(from, sideDir, axis);
    }

    float getAngle(glm::vec3 from, glm::vec3 to, glm::vec3& axis) {
        axis = glm::abs(glm::normalize(glm::cross(from, to)));
        return glm::orientedAngle(from, to, axis);
    }

}  // namespace engine