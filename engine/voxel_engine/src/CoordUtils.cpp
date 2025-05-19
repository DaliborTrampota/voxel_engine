#include "CoordUtils.h"

namespace engine {

    /// @brief Returns the chunk coordinates of the given position and modifies the position to be relative to the chunk.
    /// @return Chunk coordinate containing the position 
    ChunkID extractChunkCoords(glm::vec3& pos) {
        ChunkID chunkCoords = ChunkID(
            (int)floor(pos.x / Chunk::Dims.x),
            (int)floor(pos.y / Chunk::Dims.y),
            (int)floor(pos.z / Chunk::Dims.z)
        );

        pos.x -= chunkCoords.x * Chunk::Dims.x;
        pos.y -= chunkCoords.y * Chunk::Dims.y;
        pos.z -= chunkCoords.z * Chunk::Dims.z;

        return chunkCoords;
    }

}  // namespace engine