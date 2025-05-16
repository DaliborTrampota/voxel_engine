#include "WorldManager.h"

#include <level/World.h>
#include <level/TerrainGenerator.h>

WorldManager::WorldManager()
{
	m_currentWorld = 0;
	m_worlds[0] = std::make_unique<engine::World>(new engine::NoiseGenerator());
}

WorldManager::~WorldManager()
{
}

std::shared_ptr<engine::World> WorldManager::activeWorld()
{
    return m_worlds[m_currentWorld];
}