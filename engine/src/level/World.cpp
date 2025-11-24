#include "World.h"

#include "CoordUtils.h"
#include "ITerrainGenerator.h"
#include "block/Block.h"
#include "block/Geometry.h"
#include "data/RegistryManager.h"
#include "render/Engine.h"
#include "render/RenderContext.h"

#include <algorithm>
#include <filesystem>
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
      m_material(
          "resources/shaders/ChunkVert.glsl", "resources/shaders/ChunkFrag.glsl", "ChunkMaterial"
      ),
      m_skybox(0) {
    m_skybox.create(gl::Settings::Cubemap());

    m_skybox.loadFace(gl::CubeFace::Top, gl::ImageData("resources/skybox/top.jpg"));
    m_skybox.loadFace(gl::CubeFace::Bottom, gl::ImageData("resources/skybox/bottom.jpg"));
    m_skybox.loadFace(gl::CubeFace::Front, gl::ImageData("resources/skybox/front.jpg"));
    m_skybox.loadFace(gl::CubeFace::Back, gl::ImageData("resources/skybox/back.jpg"));
    m_skybox.loadFace(gl::CubeFace::Left, gl::ImageData("resources/skybox/left.jpg"));
    m_skybox.loadFace(gl::CubeFace::Right, gl::ImageData("resources/skybox/right.jpg"));


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

BlockID World::getBlockID(const ChunkID& chID, const glm::ivec3& pos, bool fallbackToGenerator) {
    auto chunk = m_chunks.find(chID);
    if (chunk == m_chunks.end() || !chunk->second->generated()) {
        if (fallbackToGenerator)
            return m_generator->voxelAt(pos);
        return INVALID_BLOCK;
    }

    return chunk->second->m_data.getBlock(pos);
}

BlockID World::getBlockID(glm::vec3 pos, bool fallbackToGenerator) {
    ChunkID chID = extractChunkCoords(pos);
    auto chunk = m_chunks.find(chID);
    if (chunk == m_chunks.end() || !chunk->second->generated()) {
        if (fallbackToGenerator)
            return m_generator->voxelAt(pos);
        return INVALID_BLOCK;
    }

    return chunk->second->m_data.getBlock(pos);
}

bool World::canSeeFace(const Block& curBlock, Layer layer, glm::vec3 pos, glm::ivec3 dir) const {
    glm::vec3 neighborPos = pos + glm::vec3(dir);
    ChunkID chunkCoords = engine::extractChunkCoords(neighborPos);
    if (!m_chunks.contains(chunkCoords))
        return false;

    const Chunk* chunk = getChunk(chunkCoords);
    if (!chunk)  // Chunk not generated
        return false;

    Block block = chunk->getBlock(neighborPos, layer);

    // ) || (curBlock.isVoxel() && !block.isVoxel())
    if (!curBlock.isVoxel() && block.isVoxel()) {
        dir = -dir;  // Reverse dir to find face facing the current block
        for (const auto& f : block.geometry()->faces()) {
            if (f.cullDir == dir)  // todo check if the faces are on the same plane
                return true;
        }
        return false;
    }

    bool sameBlock = block.getID() == curBlock.getID();
    if (layer == Layers::ANY)
        return sameBlock ||
               (block.isSolid() && curBlock.isSolid());  //TODO opaque instead of solid?
    return sameBlock;
}

void World::render(Engine& engine, const Camera* camera, int pass) {
    for (const ChunkID& pos : m_loadedChunks) {
        m_chunks[pos]->render(engine, camera, pass);
    }
    m_skybox.render(engine, camera);
    //std::cout << "Rendered chunks: " << m_chunks.size() << "\n";
}

void World::createChunk(ChunkID id, bool load) {
    m_chunks.emplace(id, std::make_unique<Chunk>(this, id));

    m_genPool.add([this, id, load] {
        Chunk* chunk = m_chunks[id].get();
        chunk->generate();
        chunk->generateMesh();
        if (load)
            m_loadedChunks.insert(id);
    });
}
