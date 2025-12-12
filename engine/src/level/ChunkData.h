#pragma once

#include <glm/glm.hpp>

#include <unordered_map>
#include <vector>


#include "../Globals.h"
#include "../block/Block.h"
#include "../block/BlockState.h"
#include "../block/MultiBlock.h"


#define MAKE_VEC_COMP(ret, name, op) \
    inline ret name(int x, int y, int z) const { \
        return op; \
    } \
    inline ret name(const glm::ivec3& pos) const { \
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
        glm::ivec3 dims;
        std::vector<BlockID> data;
        std::unordered_map<glm::ivec3, BlockState> states;
        std::unordered_map<glm::ivec3, MultiBlock> multiBlocks;

        ChunkData(glm::ivec3 dims) : dims(dims), data(dims.x * dims.y * dims.z, 0), states() {}

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

        // MultiBlock access
        inline MultiBlock* getMultiBlock(int x, int y, int z) {
            auto it = multiBlocks.find(glm::ivec3(x, y, z));
            return it != multiBlocks.end() ? &it->second : nullptr;
        }
        inline MultiBlock* getMultiBlock(const glm::ivec3& pos) {
            return getMultiBlock(pos.x, pos.y, pos.z);
        }

        // Set block
        inline void setBlock(int x, int y, int z, BlockID block) { data[index(x, y, z)] = block; }
        inline void setBlock(const glm::ivec3& pos, BlockID block) {
            setBlock(pos.x, pos.y, pos.z, block);
        }

        inline void setBlock(int x, int y, int z, BlockID block, BlockState state) {
            data[index(x, y, z)] = block;
            states[glm::ivec3(x, y, z)] = state;
        }
        inline void setBlock(const glm::ivec3& pos, BlockID block, BlockState state) {
            setBlock(pos.x, pos.y, pos.z, block, state);
        }

        inline void setMultiBlock(int x, int y, int z, MultiBlock&& multiBlock) {
            multiBlocks[glm::ivec3(x, y, z)] = multiBlock;
            setBlock(x, y, z, Block::MultiblockID);
            clearState(x, y, z);
        }
        inline void setMultiBlock(const glm::ivec3& pos, MultiBlock&& multiBlock) {
            setMultiBlock(pos.x, pos.y, pos.z, std::move(multiBlock));
        }

        // Clear block (set to Air)
        inline void clear(int x, int y, int z) { data[index(x, y, z)] = Block::AirID; }
        inline void clear(const glm::ivec3& pos) { clear(pos.x, pos.y, pos.z); }

        // Check if block is empty (block is Air)
        inline bool isEmpty(int x, int y, int z) const {
            return data[index(x, y, z)] == Block::AirID;
        }
        inline bool isEmpty(const glm::ivec3& pos) const { return isEmpty(pos.x, pos.y, pos.z); }

        // MAKE_VEC_COMP(bool, isEmpty, data[index(x, y, z)] == 0);

        // State access
        inline BlockState* getState(int x, int y, int z) {
            auto it = states.find(glm::ivec3(x, y, z));
            if (it == states.end()) {
                return nullptr;
            }
            return &it->second;
        }
        inline BlockState* getState(const glm::ivec3& pos) { return getState(pos.x, pos.y, pos.z); }

        inline void setState(int x, int y, int z, BlockState&& state) {
            states[glm::ivec3(x, y, z)] = std::move(state);
        }
        inline void setState(const glm::ivec3& pos, BlockState&& state) {
            setState(pos.x, pos.y, pos.z, std::move(state));
        }


        inline void clearState(int x, int y, int z) { states.erase(glm::ivec3(x, y, z)); }
        inline void clearState(const glm::ivec3& pos) { clearState(pos.x, pos.y, pos.z); }
    };

}  // namespace engine
