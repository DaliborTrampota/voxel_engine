#pragma once

#include <future>
#include <memory>
#include <unordered_map>
#include <future>

#include <glm/glm.hpp>

#include <glm/glm.hpp>

namespace engine {
    class World;
}

class WorldManager {
  public:
    WorldManager();
    ~WorldManager();

    std::shared_ptr<engine::World> activeWorld();

    std::future<void> prepareWorldForSpawn(glm::vec3 pos, glm::ivec3 ranges);


  private:
    int m_currentWorld;
    std::unordered_map<int, std::shared_ptr<engine::World>> m_worlds;
};