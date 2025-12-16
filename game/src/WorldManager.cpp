#include "WorldManager.h"

#include <level/FlatTerrainGenerator.h>
#include <level/World.h>
#include <scene/Sun.h>
#include <utility/CoordUtils.h>


#include "level/PerlinTerrainGenerator.h"


WorldManager::WorldManager(engine::Engine* engine) {
    m_currentWorld = 0;
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
