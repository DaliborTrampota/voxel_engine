#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <thread>

#include <Controls.h>
#include <physics/AABBCollider.h>
#include <scene/Updateable.h>

namespace engine {
    class World;
    class Chunk;
    class Camera;
    struct AABB;
}  // namespace engine

class Player : public engine::Updateable {
  public:
    static constexpr glm::ivec3 ViewDistance{3, 4, 3};

    Player();
    ~Player();

    void spawn(std::shared_ptr<engine::World> world);

    void update(float dt) override;

    void move(glm::vec3 key, float dt);
    void rotate(float dx, float dy, bool constrainPitch = true);

    engine::Camera* getCamera() { return m_camera.get(); }
    glm::vec3 position() const { return m_position; }

  private:
    glm::vec3 m_velocity{0, 0, 0};
    glm::vec3 m_position;
    float m_speed = 10;
    bool m_onGround = false;


    std::thread m_viewDistThread;
    engine::Chunk* m_currentChunk;

    std::shared_ptr<engine::AABB> m_aabb;
    engine::AABBCollider m_collider;
    std::unique_ptr<engine::Camera> m_camera;
};
