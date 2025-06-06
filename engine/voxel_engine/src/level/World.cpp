#include "World.h"

#include "CoordUtils.h"
#include "ITerrainGenerator.h"
#include "block/Block.h"
#include "block/Geometry.h"
#include "data/RegistryManager.h"
#include "render/RenderContext.h"
#include "render/Engine.h"

#include <algorithm>
#include <mutex>
#include <queue>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/norm.hpp>
#include <iostream>

using namespace engine;

World::World(std::unique_ptr<ITerrainGenerator> gen, uint32_t genThreads)
    : m_generator(std::move(gen)),
      m_genPool(genThreads),
      m_material("resources/shaders/ChunkVert.glsl", "resources/shaders/ChunkFrag.glsl", "ChunkMaterial") {
    printf("World created\n");
      }

World::~World() {
          printf("World deleted\n");
    m_genPool.stop();

    for (auto& [pos, chunk] : m_chunks) {
        delete chunk;
    }
}

void World::loadChunks(const glm::vec3& from, const glm::vec3& to, bool unloadRest) {
    // ChunkID chunkCoords = engine::extractChunkCoords(pos);
    if (unloadRest) {
        for (const ChunkID& id : m_loadedChunks) {
            if (id.x < from.x || id.x > to.x || id.y < from.y || id.y > to.y || id.z < from.z ||
                id.z > to.z) {
                m_loadedChunks.erase(id);
            }
        }
    }
    m_genPool.pause();
    for (int x = from.x; x < to.x; ++x) {
        for (int y = from.y; y < to.y; ++y) {
            for (int z = from.z; z < to.z; ++z) {
                ChunkID coord = ChunkID(x, y, z);
                auto it = m_chunks.find(coord);
                if (it == m_chunks.end()) {
                    createChunk(coord, false);
                }
                m_loadedChunks.insert(coord);
            }
        }
    }
    m_genPool.resume();
}

void World::unloadChunks(const glm::vec3& from, const glm::vec3& to) {
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
        for (auto it = m_loadedChunks.begin(); it != m_loadedChunks.end();) {
            it = m_loadedChunks.erase(it);
        }
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

BlockID World::getBlockID(const ChunkID& chID, const glm::ivec3& pos) {
    if (glm::any(glm::lessThan(pos, glm::ivec3(0))) ||
        glm::any(glm::greaterThan(pos, Chunk::Dims))) {
        std::cerr << "Position out of bounds\n";  // TODO add debug macros
        return INVALID_BLOCK;
    }
    auto chunk = m_chunks.find(chID);
    if (chunk == m_chunks.end() || !chunk->second->generated()) {
        //std::cerr << "Chunk not found or not generated\n";
        return INVALID_BLOCK;
    }

    return chunk->second->m_data[pos.x][pos.y][pos.z];
}

bool World::checkBlock(glm::vec3 pos, Block& curBlock, glm::ivec3 dir) const {
    ChunkID chunkCoords = engine::extractChunkCoords(pos);
    // std::cout << "chunkCoords = " << chunkCoords.x << ", " << chunkCoords.y << ", " << chunkCoords.z << "\n";
    if (!m_chunks.contains(chunkCoords))
        return false;

    Chunk* chunk = m_chunks.at(chunkCoords);
    if (!chunk)
        return false;
    Block block = chunk->getBlock(pos);


    if (!curBlock.isVoxel() && block.isVoxel() || curBlock.isVoxel() && !block.isVoxel()) {
        dir = -dir;
        for (const auto& f : block.geometry()->faces()) {
            if (f.cullDir == dir)
                return true;
        }
        return false;
    }

    return block.getID() == curBlock.getID() || block.isSolid() && curBlock.isSolid();
}

void World::render(Engine& engine, int pass) {
    for(const ChunkID& pos : m_loadedChunks) {
        m_chunks[pos]->render(engine, 0);
    }
}

void World::createChunk(ChunkID id, bool load) {
    Chunk* chunk = new Chunk(this, id);
    m_chunks[id] = chunk;

    m_genPool.add([this, chunk, load] {
        chunk->generate();
        chunk->generateMesh();
        if (load)
            m_loadedChunks.insert(chunk->id());
    });
}
