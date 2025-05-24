#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <thread>

#include <core/gl/Controls.h>
#include <entity/MovingEntity.h>

#include "Updateable.h"

namespace engine {
    class World;
    class Chunk;
    class Camera;
}  // namespace engine

class Player : public engine::MovingEntity {
  public:
    static constexpr glm::ivec3 ViewDistance{3, 4, 3};

    Player();
    ~Player();

    void spawn(std::shared_ptr<engine::World> world);

    void update(float dt) override;

    void move(engine::Key key, float dt);
    void rotate(float dx, float dy, bool constrainPitch = true);

    engine::Camera* getCamera() { return m_camera.get(); }

  private:
    std::thread m_viewDistThread;
    engine::Chunk* m_currentChunk;
    std::unique_ptr<engine::Camera> m_camera;
};
