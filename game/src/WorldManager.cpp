#include "WorldManager.h"

#include <level/FlatTerrainGenerator.h>
#include <level/World.h>
#include <scene/Sun.h>
#include <utility/CoordUtils.h>


#include "level/BiomeGenerator.h"
#include "level/PerlinTerrainGenerator.h"


#include "level/biome/biomes/Desert.h"
#include "level/biome/biomes/Forest.h"
#include "level/biome/biomes/Mountains.h"
#include "level/biome/biomes/Plains.h"
#include "level/biome/biomes/Swamp.h"


WorldManager::WorldManager(engine::Engine* engine) {
    m_currentWorld = 0;
    // m_worlds[0] =
    // std::make_unique<engine::World>(std::make_unique<engine::FlatTerrainGenerator>());
    //std::make_shared<engine::World>(std::make_unique<PerlinTerrainGenerator>(100));

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
}

WorldManager::~WorldManager() {}

std::shared_ptr<engine::World> WorldManager::activeWorld() {
    return m_worlds[m_currentWorld];
}

std::future<void> WorldManager::prepareWorldForSpawn(glm::vec3 pos, glm::ivec3 ranges) {
    ChunkID coords = engine::extractChunkCoords(pos);
    glm::ivec3 from = coords - ranges;
    glm::ivec3 to = coords + ranges;

    return m_worlds[m_currentWorld]->loadChunks(from, to);
}
