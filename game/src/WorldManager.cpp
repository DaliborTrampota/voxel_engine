#include "WorldManager.h"

#include <level/FlatTerrainGenerator.h>
#include <level/World.h>
#include <scene/Sun.h>
#include <utility/CoordUtils.h>


#include <algorithm>
#include <glm/vector_relational.hpp>


#include "level/BiomeGenerator.h"


#include "level/biome/biomes/AshPlains.h"
#include "level/biome/biomes/BasaltPlateaus.h"
#include "level/biome/biomes/CrystalFlats.h"
#include "level/biome/biomes/FracturedBasaltFields.h"
#include "level/biome/biomes/FrozenGasFields.h"
#include "level/biome/biomes/SilicateSandSeas.h"
#include "level/biome/biomes/SlateRockFields.h"

#include <level/events/LevelEvents.h>


WorldManager::WorldManager() {
    m_currentWorld = 0;
}

WorldManager::~WorldManager() {}

void WorldManager::setup() {
    BiomeGenerator generator(0);

    generator.add(std::make_unique<AshPlainsBiome>());
    generator.add(std::make_unique<BasaltPlateausBiome>());
    generator.add(std::make_unique<CrystalFlatsBiome>());
    generator.add(std::make_unique<FracturedBasaltFieldsBiome>());
    generator.add(std::make_unique<FrozenGasFieldsBiome>());
    generator.add(std::make_unique<SilicateSandSeasBiome>());
    generator.add(std::make_unique<SlateRockFieldsBiome>());

    generator.setBlendRadius(16.0f);
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
    m_worldSerializer.serialize(event->chunk);
    m_lastUnloadedChunks.push_back(event->chunk->id());
}

void WorldManager::chunkBeforeLoadEvent(engine::ChunkBeforeLoadEvent* event) {
    m_worldSerializer.deserialize(event->chunk);

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