#include "MultiBlock.h"

#include "Block.h"

using namespace engine;

MultiBlock::MultiBlock(std::initializer_list<SubBlock> blocks) : m_blocks(blocks) {}

MultiBlock MultiBlock::fromBlock(BlockID blockID, BlockState* state) {
    return MultiBlock({{blockID, state ? std::make_optional<BlockState>(*state) : std::nullopt}});
}

MultiBlock::SubBlock MultiBlock::toBlock() const {
    if (m_blocks.empty()) {
        return {Block::AirID, std::nullopt};
    }
    return m_blocks.front();
}


bool MultiBlock::hasBlock(BlockID blockID) const {
    for (const auto& block : m_blocks) {
        if (block.blockID == blockID) {
            return true;
        }
    }
    return false;
}

bool MultiBlock::addBlock(MultiBlock::SubBlock block) {
    if (block.blockID == Block::AirID || block.blockID == Block::MultiblockID)
        return false;

    if (m_blocks.size() >= 15)
        return false;

    m_blocks.reserve(m_blocks.size() + 1);
    m_blocks.push_back(block);
    return true;
}

void MultiBlock::removeBlock(uint8_t index) {
    if (index >= m_blocks.size()) {
        return;
    }
    m_blocks.erase(m_blocks.begin() + index);
}

const std::vector<MultiBlock::SubBlock>& MultiBlock::blocks() const {
    return m_blocks;
}

std::vector<BlockID> MultiBlock::blockIDs() const {
    std::vector<BlockID> blockIDs;
    for (const auto& block : m_blocks) {
        blockIDs.push_back(block.blockID);
    }
    return blockIDs;
}