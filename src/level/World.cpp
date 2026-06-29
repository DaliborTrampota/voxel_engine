#include "World.h"

#include <LWGL/texture/CubeMapArray.h>

#include "../Globals.h"
#include "ITerrainGenerator.h"
#include "block/Block.h"
#include "data/RegistryManager.h"
#include "data/TextureManager.h"
#include "events/LevelEvents.h"
#include "level/Chunk.h"
#include "level/data/DenseGrid.h"
#include "physics/Shapes.h"
#include "render/Engine.h"
#include "render/RenderContext.h"
#include "scene/Camera.h"
#include "utility/CoordUtils.h"



#include <algorithm>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/norm.hpp>

// TODO dont set chunk dirty from world, use chunk methods to set blocks

using namespace engine;

namespace {
    RegistryManager::BlockRegistryT& blocks = RegistryManager::Blocks();
}

World::World(std::unique_ptr<ITerrainGenerator> gen, uint32_t genThreads)
    : World(std::move(gen), Chunk::Dims, genThreads) {}

World::World(std::unique_ptr<ITerrainGenerator> gen, glm::ivec3 chunkDims, uint32_t genThreads)
    : m_generator(std::move(gen)),
      m_chunkDims(chunkDims),
      m_genPool(genThreads),
      m_material(
          "resources/shaders/ChunkVert.glsl", "resources/shaders/ChunkFrag.glsl", "ChunkMaterial"
      ) {
    m_material.use();
    m_material.setShadowSupport(true);
    m_material.setTexture(0, TextureManager::Get().blockTextures(), "blockTextures");
    printf("World created\n");

    m_generator->setWorld(this);
}

World::~World() {
    printf("World deleted\n");
    m_genPool.stop();
}

std::future<void> World::loadChunks(const glm::ivec3& from, const glm::ivec3& to, bool unloadRest) {
    m_genPool.pause();

    std::vector<std::shared_ptr<const Chunk>> unloadEvents;
    std::vector<Job> jobs;

    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (unloadRest) {
            for (auto it = m_loadedChunks.begin(); it != m_loadedChunks.end();) {
                const ChunkID& id = *it;
                if (id.x < from.x || id.x > to.x || id.y < from.y || id.y > to.y || id.z < from.z ||
                    id.z > to.z) {
                    auto chunkIt = m_chunks.find(id);
                    if (chunkIt != m_chunks.end()) {
                        unloadEvents.push_back(chunkIt->second);
                    }

                    it = m_loadedChunks.erase(it);
                } else {
                    ++it;
                }
            }
        }

        for (int x = from.x; x <= to.x; ++x) {
            for (int y = from.y; y <= to.y; ++y) {
                for (int z = from.z; z <= to.z; ++z) {
                    ChunkID id = ChunkID(x, y, z);
                    auto it = m_chunks.find(id);
                    if (it != m_chunks.end()) {
                        m_loadedChunks.insert(id);
                        continue;
                    }

                    auto chunk = createChunk(id);
                    m_chunks.emplace(id, chunk);

                    jobs.push_back([this, id, chunk] {
                        ChunkBeforeLoadEvent event(chunk);
                        fireChunkBeforeLoadEvent(&event);
                        chunk->populateTerrainData();
                        chunk->generateMesh();

                        std::lock_guard<std::mutex> lock(m_mutex);
                        if (m_chunks.find(id) != m_chunks.end()) {
                            m_loadedChunks.insert(id);
                        }
                    });
                }
            }
        }
    }

    if (!unloadEvents.empty()) {
        m_genPool.add([this, unloadEvents = std::move(unloadEvents)] {
            for (auto& chunk : unloadEvents) {
                ChunkUnloadEvent event(chunk);
                fireChunkUnloadEvent(&event);
            }
        });
    }

    m_genPool.resume();
    return m_genPool.addBatch(jobs);
}

void World::unloadChunks(const glm::ivec3& from, const glm::ivec3& to) {
    std::vector<std::shared_ptr<const Chunk>> unloadEvents;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (int x = from.x; x <= to.x; ++x) {
            for (int y = from.y; y <= to.y; ++y) {
                for (int z = from.z; z <= to.z; ++z) {
                    ChunkID coord = ChunkID(x, y, z);
                    auto it = m_loadedChunks.find(coord);
                    if (it != m_loadedChunks.end()) {
                        m_loadedChunks.erase(it);

                        auto chunkIt = m_chunks.find(coord);
                        if (chunkIt != m_chunks.end()) {
                            unloadEvents.push_back(chunkIt->second);
                        }
                    }
                }
            }
        }
    }

    if (!unloadEvents.empty()) {
        m_genPool.add([this, unloadEvents = std::move(unloadEvents)] {
            for (auto& chunk : unloadEvents) {
                ChunkUnloadEvent event(chunk);
                fireChunkUnloadEvent(&event);
            }
        });
    }
}

void World::unloadAllChunks(const std::vector<ChunkID>& except) {
    std::vector<std::shared_ptr<const Chunk>> unloadEvents;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (except.empty()) {
            for (auto& id : m_loadedChunks) {
                auto it = m_chunks.find(id);
                if (it != m_chunks.end()) {
                    unloadEvents.push_back(it->second);
                }
            }
            m_loadedChunks.clear();
        } else {
            for (auto it = m_loadedChunks.begin(); it != m_loadedChunks.end();) {
                if (std::find(except.begin(), except.end(), *it) == except.end()) {
                    auto chunkIt = m_chunks.find(*it);
                    if (chunkIt != m_chunks.end()) {
                        unloadEvents.push_back(chunkIt->second);
                    }
                    it = m_loadedChunks.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }

    // Fire events asynchronously AFTER releasing the lock - prevents blocking
    if (!unloadEvents.empty()) {
        m_genPool.add([this, unloadEvents = std::move(unloadEvents)] {
            for (auto& chunk : unloadEvents) {
                ChunkUnloadEvent event(chunk);
                fireChunkUnloadEvent(&event);
            }
        });
    }
}

void World::unloadChunksFromMemory(const std::vector<ChunkID>& ids) {
    m_genPool.pause();
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const ChunkID& id : ids) {
        auto it = m_chunks.find(id);
        if (it != m_chunks.end()) {
            it->second->releaseVertices(m_opaquePool, m_transparentPool);
            m_chunks.erase(it);
        }
    }
    m_genPool.resume();
}

std::shared_ptr<Chunk> World::getChunk(const ChunkID& id) {
    std::unique_lock<std::mutex> lock(m_mutex);
    auto it = m_chunks.find(id);
    if (it == m_chunks.end()) {
        return nullptr;
    }
    return it->second;
}

std::shared_ptr<const Chunk> World::getChunk(const ChunkID& id) const {
    std::unique_lock<std::mutex> lock(m_mutex);
    auto it = m_chunks.find(id);
    if (it == m_chunks.end()) {
        return nullptr;
    }
    return it->second;
}

BlockID World::getBlockID(
    const ChunkID& chID, const glm::ivec3& pos, bool fallbackToGenerator, BlockState** state
) {
    auto chunk = m_chunks.find(chID);
    if (chunk == m_chunks.end() || !chunk->second->generated()) {
        if (fallbackToGenerator)
            return m_generator->voxelAt(pos);
        return InvalidBlockID;
    }
    if (state) {
        return chunk->second->m_data->getBlockAndState(pos, *state);
    }
    return chunk->second->m_data->getBlock(pos);
}

BlockID World::getBlockID(glm::vec3 pos, bool fallbackToGenerator, BlockState** state) {
    ChunkID chID = extractChunkCoords(pos, m_chunkDims);
    auto chunk = m_chunks.find(chID);
    if (chunk == m_chunks.end() || !chunk->second->generated()) {
        if (fallbackToGenerator)
            return m_generator->voxelAt(pos);
        return InvalidBlockID;
    }
    if (state) {
        return chunk->second->m_data->getBlockAndState(pos, *state);
    }
    return chunk->second->m_data->getBlock(pos);
}

bool World::canSeeFace(const Block& curBlock, glm::vec3 pos, glm::ivec3 dir) const {
    glm::vec3 neighborPos = pos + glm::vec3(dir);
    ChunkID chunkCoords = engine::extractChunkCoords(neighborPos, m_chunkDims);

    std::shared_ptr<const Chunk> chunk = getChunk(chunkCoords);
    if (!chunk)  // Chunk not generated
        return false;

    const Block* block = RegistryManager::Blocks().get(chunk->m_data->getBlock(neighborPos));
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
            // Render opaque faces when touching transparent/translucent block
            return block->layer() != Layers::Opaque;
        case Layers::Transparent:
            // Render faces when touching different transparent blocks
            return !sameBlock && block->layer() != Layers::Opaque;
        // case Layers::Translucent:
        //     // Render translucent faces when touching different blocks (always render to show depth effect)
        //     return !sameBlock;
        case Layers::Any: return block->isSolid() && curBlock.isSolid();
        default: return false;
    }
}

void World::render(Engine& engine, const Camera* camera, int pass) {
    m_pointLightManager.update(camera);


    m_material.setVec2("resolution", engine.window()->windowSize());
    m_material.setFloat("nearPlane", camera->nearPlane());
    m_material.setFloat("farPlane", camera->farPlane());

    // Copy chunk pointers while holding lock, then render without lock
    std::vector<std::shared_ptr<Chunk>> chunksToRender;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        chunksToRender.reserve(m_loadedChunks.size());
        for (const ChunkID& pos : m_loadedChunks) {
            auto it = m_chunks.find(pos);
            if (it != m_chunks.end()) {
                chunksToRender.push_back(it->second);
            }
        }
    }

    std::array<Plane, 6> frustum = camera->getFrustum();

    m_opaqueBuffer.begin();
    m_transparentBuffer.begin();
    m_omniShadowBuffer.begin();
    for (const auto& chunk : chunksToRender) {
        if (!chunk->opaqueAlloc().isValid())
            continue;
        AABB chunkAABB{
            .min = glm::vec3(chunk->id() * m_chunkDims),
            .max = glm::vec3((chunk->id() + 1) * m_chunkDims)
        };
        for (int i = 0; i < m_pointLightManager.shadowLightCount(); i++) {
            const PointLight& light = m_pointLightManager.lights()[i];
            Sphere lightSphere(light.position, light.radius);
            if (chunkAABB.intersects(lightSphere)) {
                m_omniShadowBuffer.add(chunk->opaqueAlloc(), chunk->modelMatrix());
                break;
            }
        }
    }

    // Render without lock - shared_ptr keeps chunks alive
    for (auto& chunk : chunksToRender) {
        AABB chunkAABB{
            .min = glm::vec3(chunk->id() * m_chunkDims),
            .max = glm::vec3((chunk->id() + 1) * m_chunkDims)
        };
        bool visible = true;
        for (const auto& plane : frustum) {
            if (chunkAABB.isOutsidePlane(plane)) {
                visible = false;
                break;
            }
        }
        if (visible) {
            chunk->uploadVertices(m_opaquePool, m_transparentPool);

            glm::mat4 model = chunk->modelMatrix();
            if (chunk->opaqueAlloc().isValid())
                m_opaqueBuffer.add(chunk->opaqueAlloc(), model);
            if (chunk->transparentAlloc().isValid())
                m_transparentBuffer.add(chunk->transparentAlloc(), model);
            // chunk->render(engine, camera, pass);
        }
    }

    m_opaqueBuffer.upload();
    m_transparentBuffer.upload();
    m_omniShadowBuffer.upload();

    IndirectRenderContext ctxOpaque{
        .pool = &m_opaquePool,
        .batch = &m_opaqueBuffer,
        .material = &m_material,
        .camera = camera,
        .passMask = RenderPass::Scene | RenderPass::DirectionalShadow
    };

    IndirectRenderContext ctxTransparent{
        .pool = &m_transparentPool,
        .batch = &m_transparentBuffer,
        .material = &m_material,
        .camera = camera,
        .passMask = RenderPass::SceneTransparent | RenderPass::DirectionalShadow
    };

    IndirectRenderContext ctxOmniShadows{
        .pool = &m_opaquePool,
        .batch = &m_omniShadowBuffer,
        .material = &m_material,
        .camera = camera,
        .passMask = RenderPass::PointLightShadow
    };

    engine.submitRender(std::move(ctxOpaque));
    engine.submitRender(std::move(ctxTransparent));
    engine.submitRender(std::move(ctxOmniShadows));

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
    });
}

void World::setBlock(
    const ChunkID& chID, const glm::ivec3& pos, BlockID blockID, std::optional<BlockState> state
) {
    Chunk* chunk = nullptr;
    BlockState* storedState = nullptr;
    BlockState* oldState = nullptr;
    BlockID oldBlock = Block::AirID;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_chunks.find(chID);
        if (it == m_chunks.end())
            return;

        chunk = it->second.get();

        oldBlock = chunk->m_data->getBlock(pos);
        oldState = chunk->m_data->getState(pos);

        if (state.has_value()) {
            chunk->m_data->setBlock(pos, blockID, state.value());
            storedState = &state.value();
        } else {
            chunk->m_data->setBlock(pos, blockID);
            storedState = nullptr;
        }

        if (blockID == Block::AirID) {
            chunk->m_data->clearState(pos);
        }
        chunk->m_dirty = true;
        checkAndUpdateSurroundingChunks(chID, pos);
    }

    if (oldBlock != Block::AirID)
        blocks.get(oldBlock)->onDestroyed(
            BlockSetContext{.chunk = chunk, .position = pos, .state = oldState}
        );
    blocks.get(blockID)->onPlaced(
        BlockSetContext{.chunk = chunk, .position = pos, .state = storedState}
    );
    afterBlockSet(pos, blockID, storedState);
}

void World::setBlock(glm::ivec3 pos, BlockID blockID, std::optional<BlockState> state) {
    ChunkID chID = extractChunkCoords(pos, m_chunkDims);
    setBlock(chID, pos, blockID, state);
}

void World::setBlock(const ChunkID& chID, const glm::ivec3& pos, MultiBlock&& multiBlock) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_chunks.find(chID);
    if (it == m_chunks.end())
        return;

    Chunk* chunk = it->second.get();
    chunk->m_data->setMultiBlock(pos, std::move(multiBlock));
    chunk->m_dirty = true;
    checkAndUpdateSurroundingChunks(chID, pos);

    // TODO onPlaced?
    afterBlockSet(pos, Block::MultiblockID, nullptr);
}

void World::setBlock(glm::ivec3 pos, MultiBlock&& multiBlock) {
    ChunkID chID = extractChunkCoords(pos, m_chunkDims);
    setBlock(chID, pos, std::move(multiBlock));
}

MultiBlock* World::getMultiBlock(const ChunkID& chID, const glm::ivec3& pos) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_chunks.find(chID);
    if (it == m_chunks.end())
        return nullptr;
    return it->second->m_data->getMultiBlock(pos);
}

MultiBlock* World::getMultiBlock(glm::ivec3 pos) {
    ChunkID chID = extractChunkCoords(pos, m_chunkDims);
    return getMultiBlock(chID, pos);
}


void World::checkAndUpdateSurroundingChunks(const ChunkID& chID, const glm::ivec3& pos) {
    glm::ivec3 surroundingBlocks[] = {
        {pos.x - 1, pos.y, pos.z},
        {pos.x + 1, pos.y, pos.z},
        {pos.x, pos.y - 1, pos.z},
        {pos.x, pos.y + 1, pos.z},
        {pos.x, pos.y, pos.z - 1},
        {pos.x, pos.y, pos.z + 1}
    };
    // Cache chunk pointer to avoid redundant lookups when multiple neighbors are in same chunk
    Chunk* chunk = nullptr;
    for (auto& blockPos : surroundingBlocks) {
        blockPos += chID * m_chunkDims;
        ChunkID blockChID = getChunkID(blockPos, m_chunkDims);
        if (!chunk || blockChID != chunk->id()) {
            auto it = m_chunks.find(blockChID);
            if (it != m_chunks.end()) {
                chunk = it->second.get();
                chunk->m_dirty = true;
            }
        }
    }
}

void World::update(float dt) {
    // Copy dirty chunk IDs while holding lock, then schedule updates without lock
    std::vector<ChunkID> dirtyChunks;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (const ChunkID& pos : m_loadedChunks) {
            auto it = m_chunks.find(pos);
            if (it != m_chunks.end() && it->second->m_dirty) {
                dirtyChunks.push_back(pos);
            }
        }
    }

    // Schedule updates without lock
    for (const ChunkID& id : dirtyChunks) {
        updateChunk(id);
    }
}

std::shared_ptr<Chunk> World::createChunk(const ChunkID& id) {
    return std::make_shared<Chunk>(this, id, std::make_unique<DenseGrid>(m_chunkDims));
}

void World::setDirectionalShadowMaps(gl::TextureArray& directionalShadowMaps) {
    m_material.setTexture(1, &directionalShadowMaps, "shadowMap");
}

void World::setOmniShadowMaps(gl::CubeMapArray& omniShadowMaps) {
    m_material.setTexture(2, &omniShadowMaps, "omniShadowMaps");
}

void World::setDirectionalLight(DirectionalLight&& directionalLight) {
    m_directionalLight = std::move(directionalLight);
}