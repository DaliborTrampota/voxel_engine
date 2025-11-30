#include "WorldManager.h"

#include <CoordUtils.h>
#include <level/FlatTerrainGenerator.h>
#include <level/World.h>
#include <scene/Sun.h>

#include "level/PerlinTerrainGenerator.h"


WorldManager::WorldManager(engine::Engine* engine) {
    m_currentWorld = 0;
    m_worlds[0] =
        // std::make_unique<engine::World>(std::make_unique<engine::FlatTerrainGenerator>());
        std::make_shared<engine::World>(std::make_unique<PerlinTerrainGenerator>(100));
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
