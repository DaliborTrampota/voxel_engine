#pragma once

#include <glm/glm.hpp>
#include "../../Globals.h"
#include "../../block/Block.h"
#include "../../block/BlockState.h"

#include "../../serialize/ISerializable.h"


namespace engine {
    class MultiBlock;

    /// @brief Interface for chunk data representation.
    /// @note The concrete implementation must represent the whole chunk range, from (0, 0, 0) to (dims.x - 1, dims.y - 1, dims.z - 1).
    /// @note For representing invalid blockID use InvalidBlockID from Globals.h
    class IChunkData : public ISerializable {
      public:
        glm::ivec3 dims;
        bool populated = false;

        virtual ~IChunkData() = default;
        IChunkData(glm::ivec3 dims) : dims(dims) {}

        void serialize(std::ostream& out) const override = 0;
        void deserialize(std::istream& in) override = 0;

        /// @section Block access
        virtual BlockID getBlock(const glm::ivec3& pos) const = 0;
        virtual MultiBlock* getMultiBlock(const glm::ivec3& pos) = 0;

        virtual void setBlock(const glm::ivec3& pos, BlockID block) = 0;
        virtual void setBlock(const glm::ivec3& pos, BlockID block, BlockState state) = 0;
        virtual void setMultiBlock(const glm::ivec3& pos, MultiBlock&& multiBlock) = 0;


        /// @section State access
        virtual BlockState* getState(const glm::ivec3& pos) = 0;
        virtual const BlockState* getState(const glm::ivec3& pos) const = 0;
        virtual BlockState* getOrCreateState(const glm::ivec3& pos) = 0;

        virtual void setState(const glm::ivec3& pos, BlockState&& state) = 0;
        virtual void clearState(const glm::ivec3& pos) = 0;

        /// @section Helpers
        void clear(const glm::ivec3& pos) {
            setBlock(pos, Block::AirID);
            clearState(pos);
            // TODO clear multiblock somehow
        }

        bool isEmpty(const glm::ivec3& pos) const { return getBlock(pos) == Block::AirID; }

        BlockID getBlockAndState(const glm::ivec3& pos, BlockState*& state) {
            state = getState(pos);
            return getBlock(pos);
        }

        BlockID getBlockAndState(const glm::ivec3& pos, const BlockState*& state) const {
            state = getState(pos);
            return getBlock(pos);
        }

        BlockID getAll(const glm::ivec3& pos, BlockState*& state, MultiBlock*& multiBlock) {
            state = getState(pos);
            multiBlock = getMultiBlock(pos);
            return getBlock(pos);
        }
    };
}  // namespace engine