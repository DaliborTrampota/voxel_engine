#include "World.h"
#include <Globals.h>
#include <level/Chunk.h>

#include "ITerrainGenerator.h"
#include "block/Block.h"
#include "render/Engine.h"
#include "render/RenderContext.h"
#include "utility/CoordUtils.h"

#include <algorithm>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/norm.hpp>

using namespace engine;

World::World(std::unique_ptr<ITerrainGenerator> gen, uint32_t genThreads)
    : m_generator(std::move(gen)),
      m_genPool(genThreads),
      m_material(
          "resources/shaders/ChunkVert.glsl", "resources/shaders/ChunkFrag.glsl", "ChunkMaterial"
      ) {
    m_material.setShadowSupport(true);
    printf("World created\n");
}

World::~World() {
    printf("World deleted\n");
    m_genPool.stop();

    // for (auto& [pos, chunk] : m_chunks) {
    //     delete chunk;
    // }
}

std::future<void> World::loadChunks(const glm::ivec3& from, const glm::ivec3& to, bool unloadRest) {
    // ChunkID chunkCoords = engine::extractChunkCoords(pos);
    m_genPool.pause();
    if (unloadRest) {
        for (auto it = m_loadedChunks.begin(); it != m_loadedChunks.end();) {
            const ChunkID& id = *it;
            if (id.x < from.x || id.x > to.x || id.y < from.y || id.y > to.y || id.z < from.z ||
                id.z > to.z) {
                it = m_loadedChunks.erase(it);
            } else {
                ++it;
            }
        }
    }

    std::vector<Job> jobs;
    for (int x = from.x; x < to.x; ++x) {
        for (int y = from.y; y < to.y; ++y) {
            for (int z = from.z; z < to.z; ++z) {
                ChunkID id = ChunkID(x, y, z);
                if (m_chunks.contains(id)) {
                    m_loadedChunks.insert(id);
                    continue;
                }
                m_chunks.emplace(id, std::make_unique<Chunk>(this, id));

                jobs.push_back([this, id] {
                    Chunk* chunk = m_chunks[id].get();
                    chunk->generate();
                    chunk->generateMesh();
                    m_loadedChunks.insert(id);
                });
            }
        }
    }
    m_genPool.resume();
    return m_genPool.addBatch(jobs);
}

void World::unloadChunks(const glm::ivec3& from, const glm::ivec3& to) {
    for (int x = from.x; x < to.x; ++x) {
        for (int y = from.y; y < to.y; ++y) {
            for (int z = from.z; z < to.z; ++z) {
                ChunkID coord = ChunkID(x, y, z);
                auto it = m_loadedChunks.find(coord);
                if (it != m_loadedChunks.end()) {
                    m_loadedChunks.erase(it);
                }
            }
        }
    }
}

void World::unloadAllChunks(const std::vector<ChunkID>& except) {
    if (except.empty()) {
        m_loadedChunks.clear();
    } else {
        for (auto it = m_loadedChunks.begin(); it != m_loadedChunks.end();) {
            if (std::find(except.begin(), except.end(), *it) == except.end()) {
                it = m_loadedChunks.erase(it);
            } else {
                ++it;
            }
        }
    }
}

Chunk* World::getChunk(const ChunkID& id) {
    auto it = m_chunks.find(id);
    if (it == m_chunks.end()) {
        return nullptr;
    }
    return it->second.get();
}

const Chunk* World::getChunk(const ChunkID& id) const {
    auto it = m_chunks.find(id);
    if (it == m_chunks.end()) {
        return nullptr;
    }
    return it->second.get();
}

BlockID World::getBlockID(
    const ChunkID& chID, const glm::ivec3& pos, BlockState*& state, bool fallbackToGenerator
) {
    auto chunk = m_chunks.find(chID);
    if (chunk == m_chunks.end() || !chunk->second->generated()) {
        if (fallbackToGenerator)
            return m_generator->voxelAt(pos);
        return INVALID_BLOCK;
    }

    state = chunk->second->m_data.getState(pos);
    return chunk->second->m_data.getBlock(pos);
}

BlockID World::getBlockID(glm::vec3 pos, BlockState*& state, bool fallbackToGenerator) {
    ChunkID chID = extractChunkCoords(pos);
    auto chunk = m_chunks.find(chID);
    if (chunk == m_chunks.end() || !chunk->second->generated()) {
        if (fallbackToGenerator)
            return m_generator->voxelAt(pos);
        return INVALID_BLOCK;
    }

    state = chunk->second->m_data.getState(pos);
    return chunk->second->m_data.getBlock(pos);
}

bool World::canSeeFace(const Block& curBlock, glm::vec3 pos, glm::ivec3 dir) const {
    glm::vec3 neighborPos = pos + glm::vec3(dir);
    ChunkID chunkCoords = engine::extractChunkCoords(neighborPos);
    if (!m_chunks.contains(chunkCoords))
        return false;

    const Chunk* chunk = getChunk(chunkCoords);
    if (!chunk)  // Chunk not generated
        return false;

    const Block* block = chunk->getBlock(neighborPos);
    if (block->getID() == 0)
        return true;

    // ) || (curBlock.isVoxel() && !block.isVoxel())
    if (!curBlock.isVoxel() && block->isVoxel()) {
        return true;
        // TODO figure out, either ignore and always draw face or its gonna be pain and check if faces are on same plane and if one contains the other and draw only the bigger
        // dir = -dir;  // Reverse dir to find face facing the current block
        // for (const auto& f : block.geometry()->faces()) {
        //     if (f.cullDir == dir)  // todo check if the faces are on the same plane
        //         return true;
        // }
        // return false;
    } else if (curBlock.isVoxel() && block->isVoxel()) {
        // TODO is it worth figuring out which faces should not be rendered?
        return true;
    }

    bool sameBlock = block->getID() == curBlock.getID();
    if (sameBlock)
        return false;

    switch (curBlock.layer()) {
        case Layers::Opaque:
            // Render opaque faces when touching transparent block
            return block->layer() != Layers::Opaque;
        case Layers::Transparent:
            // Render faces when touching different transparent blocks
            return !sameBlock && block->layer() != Layers::Opaque;
        case Layers::Any: return block->isSolid() && curBlock.isSolid();
        default: return false;
    }
}

void World::render(Engine& engine, const Camera* camera, int pass) {
    for (const ChunkID& pos : m_loadedChunks) {
        m_chunks[pos]->render(engine, camera, pass);
    }
    m_skybox.render(engine, camera);
    //std::cout << "Rendered chunks: " << m_chunks.size() << "\n";
}

void World::updateChunk(ChunkID id) {
    m_genPool.add([this, id] {
        std::unique_lock<std::mutex> lock(m_mutex);
        auto it = m_chunks.find(id);
        if (it == m_chunks.end()) {
            return;
        }
        Chunk* chunk = it->second.get();
        if (!chunk)
            return;

        lock.unlock();
        chunk->generateMesh();
        chunk->m_dirty = false;
    });
}

void World::setBlock(
    const ChunkID& chID, const glm::ivec3& pos, BlockID blockID, std::optional<BlockState> state
) {
    if (state.has_value())
        m_chunks[chID]->m_data.setBlock(pos, blockID, state.value());
    else
        m_chunks[chID]->m_data.setBlock(pos, blockID);

    if (blockID == 0) {
        m_chunks[chID]->m_data.clearState(pos);
    }
    m_chunks[chID]->m_dirty = true;
    checkAndUpdateSurroundingChunks(chID, pos);
}

void World::setBlock(glm::ivec3 pos, BlockID blockID, std::optional<BlockState> state) {
    ChunkID chID = extractChunkCoords(pos);
    setBlock(chID, pos, blockID, state);
}

void World::setBlock(const ChunkID& chID, const glm::ivec3& pos, MultiBlock&& multiBlock) {
    m_chunks[chID]->m_data.setMultiBlock(pos, std::move(multiBlock));
    m_chunks[chID]->m_dirty = true;
    checkAndUpdateSurroundingChunks(chID, pos);
}

MultiBlock* World::getMultiBlock(const ChunkID& chID, const glm::ivec3& pos) {
    return m_chunks[chID]->m_data.getMultiBlock(pos);
}

MultiBlock* World::getMultiBlock(glm::ivec3 pos) {
    ChunkID chID = extractChunkCoords(pos);
    return getMultiBlock(chID, pos);
}


void World::setBlock(glm::ivec3 pos, MultiBlock&& multiBlock) {
    ChunkID chID = extractChunkCoords(pos);
    setBlock(chID, pos, std::move(multiBlock));
}

//TODO improve
void World::checkAndUpdateSurroundingChunks(const ChunkID& chID, const glm::ivec3& pos) {
    glm::ivec3 surroundingBlocks[] = {
        {pos.x - 1, pos.y, pos.z},
        {pos.x + 1, pos.y, pos.z},
        {pos.x, pos.y - 1, pos.z},
        {pos.x, pos.y + 1, pos.z},
        {pos.x, pos.y, pos.z - 1},
        {pos.x, pos.y, pos.z + 1}
    };
    for (auto& blockPos : surroundingBlocks) {
        blockPos += chID * Chunk::Dims;
        ChunkID blockChID = getChunkID(blockPos);
        auto it = m_chunks.find(blockChID);
        if (it != m_chunks.end()) {
            it->second->m_dirty = true;
        }
    }
}

void World::update(float dt) {
    for (const ChunkID& pos : m_loadedChunks) {
        if (m_chunks[pos]->m_dirty) {
            updateChunk(pos);
        }
    }
}