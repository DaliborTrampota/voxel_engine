#pragma once

#include "../Globals.h"
// #include "Placeable.h"
#include "BlockState.h"


#include <initializer_list>
#include <optional>
#include <vector>


namespace engine {
    /// @brief A MultiBlock represents multiple blocks at the same position.
    /// @note Each block can also have it's own state, just like normal blocks.
    /// @note It can have up to 15 blocks.
    /// @note It can have Blocks and VariantBlocks.
    /// @warning It is user's responsibility to ensure the blocks make sense together (visibility, overlapping, z-fighting, etc.)
    class MultiBlock {
      public:
        /// @note Default constructor should not be used. Use fromBlock to create a MultiBlock.
        MultiBlock() = default;
        ~MultiBlock() = default;

        struct SubBlock {
            BlockID blockID;
            std::optional<BlockState> state;
        };

        /// @brief Converts a block and its state to a multi-block.
        /// @param blockID The ID of the block to convert.
        /// @param state The state of the block to convert.
        static MultiBlock fromBlock(BlockID, BlockState* state);

        /// @brief Converts a MultiBlock to a block and its state.
        /// @note Makes the first subblock the base block.
        /// @return SubBlock consisting from BlockID and its state. If MultiBlock is empty, returns air, if MultiBlock has more than one SubBlocks, returns the first SubBlock.
        SubBlock toBlock() const;


        bool hasBlock(BlockID blockID) const;
        bool addBlock(SubBlock block);
        void removeBlock(uint8_t index);

        const std::vector<SubBlock>& blocks() const;
        std::vector<BlockID> blockIDs() const;
        uint8_t blockCount() const { return m_blocks.size(); }


      protected:
        MultiBlock(std::initializer_list<SubBlock> blocks);

      private:
        std::vector<SubBlock> m_blocks;
    };
}  // namespace engine