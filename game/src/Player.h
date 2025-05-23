#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <thread>

#include <core/gl/Controls.h>
#include <physics/AABB.h>
#include <physics/AABBCollider.h>

namespace engine {
    class World;
    class Chunk;
    class Camera;
}  // namespace engine

class Player {
  public:
    static constexpr glm::ivec3 ViewDistance{3, 4, 3};

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

    engine::AABB m_aabb{
        glm::vec3(-0.5f, 0.0f, -0.5f),
        glm::vec3(0.5f, 1.8f, 0.5f),
    };
    engine::AABBCollider m_collider;
};
