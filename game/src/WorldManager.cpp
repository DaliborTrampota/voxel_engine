#include "WorldManager.h"

#include <level/FlatTerrainGenerator.h>
#include <level/PerlinTerrainGenerator.h>
#include <level/World.h>

WorldManager::WorldManager() {
    m_currentWorld = 0;
    m_worlds[0] =
        // std::make_unique<engine::World>(std::make_unique<engine::FlatTerrainGenerator>());
        std::make_unique<engine::World>(std::make_unique<engine::PerlinTerrainGenerator>(100));
}

WorldManager::~WorldManager() {}

std::shared_ptr<engine::World> WorldManager::activeWorld() {
    return m_worlds[m_currentWorld];
}