#include "DenseGrid.h"

#include <iostream>

using namespace engine;

BlockID DenseGrid::getBlock(const glm::ivec3& pos) const {
    return data[index(pos)];
}

// MultiBlock access
MultiBlock* DenseGrid::getMultiBlock(const glm::ivec3& pos) {
    auto it = multiBlocks.find(pos);
    return it != multiBlocks.end() ? &it->second : nullptr;
}

// Set block
void DenseGrid::setBlock(const glm::ivec3& pos, BlockID block) {
    data[index(pos)] = block;
}

void DenseGrid::setBlock(const glm::ivec3& pos, BlockID block, BlockState state) {
    data[index(pos)] = block;
    states[pos] = state;
}

void DenseGrid::setMultiBlock(const glm::ivec3& pos, MultiBlock&& multiBlock) {
    multiBlocks[pos] = multiBlock;
    setBlock(pos, Block::MultiblockID);
    clearState(pos);
}

// State access
BlockState* DenseGrid::getState(const glm::ivec3& pos) {
    auto it = states.find(pos);
    if (it == states.end()) {
        return nullptr;
    }
    return &it->second;
}

const BlockState* DenseGrid::getState(const glm::ivec3& pos) const {
    auto it = states.find(pos);
    if (it == states.end()) {
        return nullptr;
    }
    return &it->second;
}

void DenseGrid::setState(const glm::ivec3& pos, BlockState&& state) {
    states[pos] = std::move(state);
}

void DenseGrid::clearState(const glm::ivec3& pos) {
    states.erase(pos);
}

BlockState* DenseGrid::getOrCreateState(const glm::ivec3& pos) {
    auto [it, inserted] = states.try_emplace(pos);
    return &it->second;
}

}