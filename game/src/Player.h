#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <thread>

#include <core/gl/Controls.h>

namespace engine {
    class World;
    class Chunk;
    class Camera;
}  // namespace engine

class Player {
  public:
    Player();
    ~Player();

    void spawn(std::shared_ptr<engine::World> world);

    void move(engine::Key key, float dt);
    void rotate(float dx, float dy, bool constrainPitch = true);

    engine::Camera* getCamera() { return m_camera; }

  private:
    std::thread m_viewDistThread;

    engine::Camera* m_camera;
    glm::vec3 m_position;
    engine::Chunk* m_currentChunk;
};
