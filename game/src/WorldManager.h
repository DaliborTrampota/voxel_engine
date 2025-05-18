#pragma once

#include <memory>
#include <unordered_map>

namespace engine {
    class World;
}

class WorldManager {
  public:
    WorldManager();
    ~WorldManager();

    std::shared_ptr<engine::World> activeWorld();


  private:
    int m_currentWorld;
    std::unordered_map<int, std::shared_ptr<engine::World>> m_worlds;
};