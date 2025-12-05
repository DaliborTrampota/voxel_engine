#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "../Globals.h"

namespace engine {

    /**
     * ChunkData stores voxel information as BlockIDs.
     */
    struct ChunkData {
        bool populated = false;
        glm::ivec3 dims;
        std::vector<BlockID> data;

        ChunkData() = default;
        ChunkData(glm::ivec3 dims) : dims(dims), data(dims.x * dims.y * dims.z, 0) {}

        // 3D to 1D index mapping (Z-Y-X order for cache locality)
        inline size_t index(int x, int y, int z) const { return x + dims.x * (y + dims.y * z); }

        inline size_t index(const glm::ivec3& pos) const { return index(pos.x, pos.y, pos.z); }

        // Block access
        inline BlockID& operator()(int x, int y, int z) { return data[index(x, y, z)]; }

        inline const BlockID& operator()(int x, int y, int z) const { return data[index(x, y, z)]; }

        inline BlockID& operator()(const glm::ivec3& pos) { return data[index(pos)]; }

        inline const BlockID& operator()(const glm::ivec3& pos) const { return data[index(pos)]; }

        // Block ID access
        inline BlockID getBlock(int x, int y, int z) const { return data[index(x, y, z)]; }

        inline BlockID getBlock(const glm::ivec3& pos) const {
            return getBlock(pos.x, pos.y, pos.z);
        }

        // Set block
        inline void setBlock(int x, int y, int z, BlockID block) { data[index(x, y, z)] = block; }

        inline void setBlock(const glm::ivec3& pos, BlockID block) {
            setBlock(pos.x, pos.y, pos.z, block);
        }

        // Clear voxel (set to 0)
        inline void clear(int x, int y, int z) { data[index(x, y, z)] = 0; }

        inline void clear(const glm::ivec3& pos) { clear(pos.x, pos.y, pos.z); }

        // Check if voxel is empty (block is 0 / air)
        inline bool isEmpty(int x, int y, int z) const { return data[index(x, y, z)] == 0; }

        inline bool isEmpty(const glm::ivec3& pos) const { return isEmpty(pos.x, pos.y, pos.z); }
    };

}  // namespace engine
