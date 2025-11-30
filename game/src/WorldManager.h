#pragma once

#include <future>
#include <memory>
#include <unordered_map>

#include <glm/glm.hpp>

#include <glm/glm.hpp>

namespace engine {
    class Engine;
    class World;
    class Sun;
}  // namespace engine

class WorldManager {
  public:
    WorldManager(engine::Engine* engine);
    ~WorldManager();

    std::shared_ptr<engine::World> activeWorld();

    std::future<void> prepareWorldForSpawn(glm::vec3 pos, glm::ivec3 ranges);


  private:
    int m_currentWorld;
    std::unordered_map<int, std::shared_ptr<engine::World>> m_worlds;
};