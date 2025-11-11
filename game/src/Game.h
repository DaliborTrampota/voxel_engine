#pragma once

#include <deque>
#include <unordered_map>

#include <LWGL/buffer/UBO.h>
#include <render/Engine.h>

#include "Player.h"
#include "WorldManager.h"
#include "ui/UIManager.h"

namespace gl {
    class Window;
}

namespace ui {
    class Renderer;
}

using namespace engine;

namespace engine {
    class Camera;
    class InputSystem;
}  // namespace engine

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
    void afterRender() override;

  private:
    std::unique_ptr<InputSystem> m_inputSystem;

    gl::UBO m_commonUBO;

    std::shared_ptr<Player> m_player;
    Camera* m_plrCamera;
    WorldManager m_worldManager;

    std::shared_ptr<UIManager> m_uiManager;
};