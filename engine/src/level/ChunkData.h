#pragma once

#include <glm/glm.hpp>

#include <unordered_map>
#include <vector>


#include "../Globals.h"
#include "../block/Block.h"
#include "../block/BlockState.h"
#include "../block/MultiBlock.h"


#define MAKE_VEC_COMP(ret, name, op) \
    ret name(int x, int y, int z) const { \
        return op; \
    } \
    ret name(const glm::ivec3& pos) const { \
        return name(pos.x, pos.y, pos.z); \
    }

namespace std {
    template <>
    struct hash<glm::ivec3> {
        size_t operator()(const glm::ivec3& k) const {
            return hash<int>()(k.x) ^ hash<int>()(k.y << 1) ^ hash<int>()(k.z << 2);
        }
    };

}  // namespace std

namespace engine {
    // TODO probably move impl to vec3 version forward from comps, but it shouldnt matter for performance, users are more likely to use vec3s
    /**
     * ChunkData stores voxel information as BlockIDs.
     */
    struct ChunkData {
        bool populated = false;
        BlockID uniform = InvalidBlockID;
        glm::ivec3 dims;
        std::vector<BlockID> data;
        std::unordered_map<glm::ivec3, BlockState> states;
        std::unordered_map<glm::ivec3, MultiBlock> multiBlocks;

        ChunkData(glm::ivec3 dims) : dims(dims), data(dims.x * dims.y * dims.z, 0), states() {}

        void compress(BlockID block) {
            uniform = block;
            data.resize(0);
            multiBlocks.clear();
            // TODO handle states?
        }

        void decompress() {
            data.resize(dims.x * dims.y * dims.z, uniform);
            uniform = InvalidBlockID;
        }

        // 3D to 1D index mapping (Z-Y-X order for cache locality)
        size_t index(int x, int y, int z) const { return x + dims.x * (y + dims.y * z); }
        size_t index(const glm::ivec3& pos) const { return index(pos.x, pos.y, pos.z); }


        /// @section Block access
        // BlockID& operator()(const glm::ivec3& pos) { return data[index(pos)]; }
        // const BlockID& operator()(const glm::ivec3& pos) const { return data[index(pos)]; }
        BlockID getBlock(const glm::ivec3& pos) const;
        MultiBlock* getMultiBlock(const glm::ivec3& pos);

        /// @section Set block
        void setBlock(const glm::ivec3& pos, BlockID block);
        void setBlock(const glm::ivec3& pos, BlockID block, BlockState state);
        void setMultiBlock(const glm::ivec3& pos, MultiBlock&& multiBlock);

        void clear(const glm::ivec3& pos);
        bool isEmpty(const glm::ivec3& pos) const;

        // MAKE_VEC_COMP(bool, isEmpty, data[index(x, y, z)] == 0);

        /// @section State access
        BlockState* getState(const glm::ivec3& pos);
        const BlockState* getState(const glm::ivec3& pos) const;

        void setState(const glm::ivec3& pos, BlockState&& state);
        void clearState(const glm::ivec3& pos);

        /// @section Helpers
        BlockState* getOrCreateState(const glm::ivec3& pos);
        BlockID getBlockAndState(const glm::ivec3& pos, BlockState*& state);
        BlockID getBlockAndState(const glm::ivec3& pos, const BlockState*& state) const;
        BlockID getAll(const glm::ivec3& pos, BlockState*& state, MultiBlock*& multiBlock);
    };

}  // namespace engine
