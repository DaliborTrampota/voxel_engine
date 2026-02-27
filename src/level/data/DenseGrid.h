#pragma once

#include "../../hash.h"
#include "IChunkData.h"


#include <glm/glm.hpp>

#include <unordered_map>
#include <vector>


#include "../../block/BlockState.h"
#include "../../block/MultiBlock.h"


#define MAKE_VEC_COMP(ret, name, op) \
    ret name(int x, int y, int z) const { \
        return op; \
    } \
    ret name(const glm::ivec3& pos) const { \
        return name(pos.x, pos.y, pos.z); \
    }


namespace engine {
    /// @brief DenseGrid is a storage of blocks in a dense grid aka array..
    struct DenseGrid : public IChunkData {
        bool populated = false;
        BlockID uniform = InvalidBlockID;
        std::vector<BlockID> data;
        std::unordered_map<glm::ivec3, BlockState> states;
        std::unordered_map<glm::ivec3, MultiBlock> multiBlocks;

        DenseGrid(glm::ivec3 dims)
            : IChunkData(dims),
              data(dims.x * dims.y * dims.z, 0),
              states() {}

        void compress(BlockID block) {
            uniform = block;
            data.resize(0);
            multiBlocks.clear();
            // Keep states as is
        }

        void decompress() {
            data.resize(dims.x * dims.y * dims.z, uniform);
            uniform = InvalidBlockID;
        }

        // 3D to 1D index mapping (Z-Y-X order for cache locality)
        size_t index(int x, int y, int z) const { return x + dims.x * (y + dims.y * z); }
        size_t index(const glm::ivec3& pos) const { return index(pos.x, pos.y, pos.z); }


        /// @section Block access
        BlockID getBlock(const glm::ivec3& pos) const override final;
        MultiBlock* getMultiBlock(const glm::ivec3& pos) override final;

        void setBlock(const glm::ivec3& pos, BlockID block) override final;
        void setBlock(const glm::ivec3& pos, BlockID block, BlockState state) override final;
        void setMultiBlock(const glm::ivec3& pos, MultiBlock&& multiBlock) override final;


        /// @section State access
        BlockState* getState(const glm::ivec3& pos) override final;
        const BlockState* getState(const glm::ivec3& pos) const override final;

        void setState(const glm::ivec3& pos, BlockState&& state) override final;
        void clearState(const glm::ivec3& pos) override final;
        BlockState* getOrCreateState(const glm::ivec3& pos) override final;

        void serialize(std::ostream& out) const override final;
        void deserialize(std::istream& in) override final;
    };

}  // namespace engine
