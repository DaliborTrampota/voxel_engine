#pragma once

#include <deque>
#include <unordered_map>

#include <LWGL/buffer/UBO.h>
#include <LWGL/events/GLEventSite.h>

#include <InputSystem.h>
#include <render/Engine.h>

#include "Player.h"
#include "WorldManager.h"

namespace gl {
    class Window;
}

using namespace engine;

namespace engine {
    class Camera;
}

struct MouseState {
    MouseState() = delete;
    MouseState(glm::ivec2 dims) : lastX(dims.x / 2.0f), lastY(dims.y / 2.0f), firstMouse(true) {}
    float lastX, lastY;
    bool firstMouse;
};

class Game : public engine::Engine {
  public:
    Game() = delete;
    Game(std::unique_ptr<gl::Window> window, glm::ivec2 dims);
    ~Game();

    void start();

    const std::weak_ptr<Player> getPlayer() const { return m_player; }
    std::shared_ptr<World> activeWorld() { return m_worldManager.activeWorld(); }

    void processInput();
    void render(double dt) override;

  private:
    std::unique_ptr<InputSystem> m_inputSystem;
    MouseState m_mouseState;

    gl::UBO m_commonUBO;

    std::shared_ptr<Player> m_player;
    Camera* m_plrCamera;
    WorldManager m_worldManager;
};