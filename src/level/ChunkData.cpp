#include "ChunkData.h"

#include "block/Block.h"

using namespace engine;

BlockID ChunkData::getBlock(const glm::ivec3& pos) const {
    return data[index(pos)];
}

// MultiBlock access
MultiBlock* ChunkData::getMultiBlock(const glm::ivec3& pos) {
    auto it = multiBlocks.find(pos);
    return it != multiBlocks.end() ? &it->second : nullptr;
}

// Set block
void ChunkData::setBlock(const glm::ivec3& pos, BlockID block) {
    data[index(pos)] = block;
}

void ChunkData::setBlock(const glm::ivec3& pos, BlockID block, BlockState state) {
    data[index(pos)] = block;
    states[pos] = state;
}

void ChunkData::setMultiBlock(const glm::ivec3& pos, MultiBlock&& multiBlock) {
    multiBlocks[pos] = multiBlock;
    setBlock(pos, Block::MultiblockID);
    clearState(pos);
}

// Clear block (set to Air)
void ChunkData::clear(const glm::ivec3& pos) {
    data[index(pos)] = Block::AirID;
}

bool ChunkData::isEmpty(const glm::ivec3& pos) const {
    return data[index(pos)] == Block::AirID;
}


// State access
BlockState* ChunkData::getState(const glm::ivec3& pos) {
    auto it = states.find(pos);
    if (it == states.end()) {
        return nullptr;
    }
    return &it->second;
}

const BlockState* ChunkData::getState(const glm::ivec3& pos) const {
    auto it = states.find(pos);
    if (it == states.end()) {
        return nullptr;
    }
    return &it->second;
}

void ChunkData::setState(const glm::ivec3& pos, BlockState&& state) {
    states[pos] = std::move(state);
}

void ChunkData::clearState(const glm::ivec3& pos) {
    states.erase(pos);
}

// Helpers


BlockState* ChunkData::getOrCreateState(const glm::ivec3& pos) {
    auto [it, inserted] = states.try_emplace(pos);
    return &it->second;
}

BlockID ChunkData::getBlockAndState(const glm::ivec3& pos, BlockState*& state) {
    state = getState(pos);
    return getBlock(pos);
}

BlockID ChunkData::getBlockAndState(const glm::ivec3& pos, const BlockState*& state) const {
    state = getState(pos);
    return getBlock(pos);
}

BlockID ChunkData::getAll(const glm::ivec3& pos, BlockState*& state, MultiBlock*& multiBlock) {
    state = getState(pos);
    multiBlock = getMultiBlock(pos);
    return getBlock(pos);
}