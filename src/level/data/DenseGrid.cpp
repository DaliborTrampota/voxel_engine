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
    if (block != Block::MultiblockID) {
        multiBlocks.erase(pos);
    }
    data[index(pos)] = block;
}

void DenseGrid::setBlock(const glm::ivec3& pos, BlockID block, BlockState state) {
    if (block != Block::MultiblockID) {
        multiBlocks.erase(pos);
    }
    data[index(pos)] = block;
    states[pos] = state;
}

void DenseGrid::setMultiBlock(const glm::ivec3& pos, MultiBlock&& multiBlock) {
    multiBlocks[pos] = std::move(multiBlock);
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

// TODO (de)serialize multiblocks
void DenseGrid::serialize(std::ostream& out) const {
    std::vector<std::pair<glm::ivec3, engine::BlockState>> serializedStates;

    for (int x = 0; x < dims.x; x++) {
        for (int y = 0; y < dims.y; y++) {
            for (int z = 0; z < dims.z; z++) {
                const engine::BlockState* state = nullptr;
                out << getBlockAndState({x, y, z}, state) << " ";
                if (state) {
                    serializedStates.push_back({glm::ivec3(x, y, z), *state});
                }
            }
        }
    }

    out << serializedStates.size() << std::endl;
    for (const auto& [pos, state] : serializedStates) {
        out << pos.x << " " << pos.y << " " << pos.z << " ";
        state.serialize(out);
        out << std::endl;
    }
}

void DenseGrid::deserialize(std::istream& in) {
    // clear the data
    data.assign(dims.x * dims.y * dims.z, Block::AirID);
    states = {};
    multiBlocks = {};

    engine::BlockID block;
    for (int x = 0; x < dims.x; x++) {
        for (int y = 0; y < dims.y; y++) {
            for (int z = 0; z < dims.z; z++) {
                in >> block;
                setBlock({x, y, z}, block);
            }
        }
    }

    size_t stateCount;
    in >> stateCount;
    for (size_t i = 0; i < stateCount; ++i) {
        glm::ivec3 pos;
        in >> pos.x >> pos.y >> pos.z;

        engine::BlockState state;
        state.deserialize(in);
        setState(pos, std::move(state));
    }

    populated = true;
}