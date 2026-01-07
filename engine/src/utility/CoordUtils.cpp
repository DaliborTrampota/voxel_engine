#include "CoordUtils.h"

#include <glm/gtc/constants.hpp>
#include "glm/geometric.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/vector_angle.hpp>

namespace engine {

    ChunkID extractChunkCoords(glm::vec3& pos) {
        return extractChunkCoords(pos, Chunk::Dims);
    }

    ChunkID extractChunkCoords(glm::ivec3& pos) {
        return extractChunkCoords(pos, Chunk::Dims);
    }

    ChunkID extractChunkCoords(glm::vec3& pos, const glm::ivec3& chunkDims) {
        ChunkID chunkCoords =
            ChunkID(static_cast<glm::ivec3>(glm::floor(pos / glm::vec3(chunkDims))));
        pos -= chunkCoords * chunkDims;
        return chunkCoords;
    }

    ChunkID extractChunkCoords(glm::ivec3& pos, const glm::ivec3& chunkDims) {
        ChunkID chunkCoords = ChunkID(
            floorDiv(pos.x, chunkDims.x), floorDiv(pos.y, chunkDims.y), floorDiv(pos.z, chunkDims.z)
        );
        pos -= chunkCoords * chunkDims;
        return chunkCoords;
    }

    glm::ivec3 toChunkCoords(const ChunkID& chunkID, const glm::vec3& pos) {
        return toChunkCoords(chunkID, pos, Chunk::Dims);
    }

    glm::ivec3 toChunkCoords(
        const ChunkID& chunkID, const glm::vec3& pos, const glm::ivec3& chunkDims
    ) {
        return static_cast<glm::ivec3>(glm::floor(pos)) - chunkID * chunkDims;
    }

    ChunkID getChunkID(const glm::vec3& pos) {
        return getChunkID(pos, Chunk::Dims);
    }

    ChunkID getChunkID(const glm::vec3& pos, const glm::ivec3& chunkDims) {
        return ChunkID(static_cast<glm::ivec3>(glm::floor(pos / glm::vec3(chunkDims))));
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
}  // namespace engine