#pragma once

#include <glm/glm.hpp>
#include <memory>

#include <level/Chunk.h>
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

    void updateViewDistance(engine::ChunkID center, bool waitTillLoaded);

    void update(float dt) override;

    void move(glm::vec3 lDir, float dt);
    void rotate(float dx, float dy, bool constrainPitch = true);
    void move(glm::vec3 position);

    engine::Camera* getCamera() { return m_camera.get(); }
    const glm::vec3& position() const { return m_position; }

  private:
    glm::vec3 m_velocity{0.f};
    glm::vec3 m_position;
    float m_speed = 5.f;
    bool m_onGround = false;

    // Smooth step animation
    float m_stepAnimProgress = 1.0f;  // 1.0 = no animation playing
    float m_stepStartY = 0.0f;
    float m_stepTargetY = 0.0f;

    engine::Chunk* m_currentChunk;
    std::shared_ptr<engine::World> m_world;

    std::shared_ptr<engine::AABB> m_aabb;
    engine::AABBCollider m_collider;
    std::unique_ptr<engine::Camera> m_camera;
};
