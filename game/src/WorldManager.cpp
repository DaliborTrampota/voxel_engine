#include "WorldManager.h"

#include <level/FlatTerrainGenerator.h>
#include <level/World.h>
#include <scene/Sun.h>
#include <utility/CoordUtils.h>


#include <algorithm>
#include <glm/vector_relational.hpp>


#include "level/BiomeGenerator.h"
#include "level/PerlinTerrainGenerator.h"


#include "level/biome/biomes/Desert.h"
#include "level/biome/biomes/Forest.h"
#include "level/biome/biomes/Mountains.h"
#include "level/biome/biomes/Plains.h"
#include "level/biome/biomes/Swamp.h"

#include <level/events/LevelEvents.h>


WorldManager::WorldManager() {
    m_currentWorld = 0;
}

WorldManager::~WorldManager() {}

void WorldManager::setup() {
    BiomeGenerator generator(0);
    generator.add(std::make_unique<PlainsBiome>());
    generator.add(std::make_unique<MountainBiome>());
    generator.add(std::make_unique<DesertBiome>());
    generator.add(std::make_unique<ForestBiome>());
    generator.add(std::make_unique<SwampBiome>());
    generator.setBlendRadius(32.0f);
    generator.setEnableBlending(true);

    m_worlds[0] =
        std::make_shared<engine::World>(std::make_unique<BiomeGenerator>(std::move(generator)));

    Skybox::Settings planetSkyboxSettings = {
        .top = "resources/textures/skybox/v1_top.png",
        .bottom = "resources/textures/skybox/v1_bottom.png",
        .front = "resources/textures/skybox/v1_front.png",
        .back = "resources/textures/skybox/v1_back.png",
        .left = "resources/textures/skybox/v1_left.png",
        .right = "resources/textures/skybox/v1_right.png",
    };
    m_worlds[0]->getSkybox().load(planetSkyboxSettings);


    m_worlds[0]->subscribe(this);
}

std::shared_ptr<engine::World> WorldManager::activeWorld() {
    return m_worlds[m_currentWorld];
}

std::future<void> WorldManager::prepareWorldForSpawn(glm::vec3 pos, glm::ivec3 ranges) {
    ChunkID coords = engine::extractChunkCoords(pos);
    glm::ivec3 from = coords - ranges;
    glm::ivec3 to = coords + ranges;

    return m_worlds[m_currentWorld]->loadChunks(from, to);
}


void WorldManager::chunkUnloadEvent(engine::ChunkUnloadEvent* event) {
    m_worldSerializer.serialize(*event->chunk);
    m_lastUnloadedChunks.push_back(event->chunk->id());
}

void WorldManager::chunkBeforeLoadEvent(engine::ChunkBeforeLoadEvent* event) {
    m_worldSerializer.deserialize(*event->chunk);

    auto it =
        std::find(m_lastUnloadedChunks.begin(), m_lastUnloadedChunks.end(), event->chunk->id());
    if (it != m_lastUnloadedChunks.end()) {
        m_lastUnloadedChunks.erase(it);
    }
}

void WorldManager::cleanupUnloadedChunks(glm::ivec3 around, glm::ivec3 distance) {
    auto removeBegin = std::remove_if(
        m_lastUnloadedChunks.begin(),
        m_lastUnloadedChunks.end(),
        [around, distance](ChunkID chunkID) {
            return glm::any(glm::greaterThan(glm::abs(chunkID - around), distance));
        }
    );
    activeWorld()->unloadChunksFromMemory({removeBegin, m_lastUnloadedChunks.end()});
    m_lastUnloadedChunks.erase(removeBegin, m_lastUnloadedChunks.end());
}