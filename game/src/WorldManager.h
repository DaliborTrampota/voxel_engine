#pragma once

#include <future>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include <level/Chunk.h>
#include <level/events/LevelEventSite.h>
#include <glm/glm.hpp>


#include "serializer/WorldSerializer.h"


namespace engine {
    class World;
}  // namespace engine

class WorldManager : public engine::LevelEventSite {
  public:
    WorldManager();
    ~WorldManager();

    void setup();
    std::shared_ptr<engine::World> activeWorld();
    std::future<void> prepareWorldForSpawn(glm::vec3 pos, glm::ivec3 ranges);

    void chunkUnloadEvent(engine::ChunkUnloadEvent* event);
    void chunkBeforeLoadEvent(engine::ChunkBeforeLoadEvent* event);

    void cleanupUnloadedChunks(glm::ivec3 around, glm::ivec3 distance);

  private:
    int m_currentWorld;
    std::unordered_map<int, std::shared_ptr<engine::World>> m_worlds;

    WorldSerializer m_worldSerializer;
    std::vector<engine::ChunkID> m_lastUnloadedChunks;
};