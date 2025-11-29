#pragma once

#include <LWGL/buffer/UBO.h>
#include <render/Engine.h>

#include "Player.h"
#include "WorldManager.h"
#include "ui/UIManager.h"

#include <GLFWUserPointer.h>

using namespace engine;

namespace ui {
    class Renderer;
}

namespace engine {
    class Camera;
    class InputSystem;
    class Window;
}  // namespace engine

class Game : public engine::Engine {
  public:
    Game() = delete;
    Game(std::unique_ptr<Window> window, glm::ivec2 dims);
    ~Game();

    void start();

    const std::weak_ptr<Player> getPlayer() const { return m_player; }
    std::shared_ptr<World> activeWorld() { return m_worldManager.activeWorld(); }

    void processInput();
    void render(double dt) override;
    void afterRender() override;


    void renderShadowMapDebug();

  private:
    std::unique_ptr<InputSystem> m_inputSystem;
    GLFWUserPointer m_pointer;

    gl::UBO m_commonUBO;

    std::shared_ptr<Player> m_player;
    Camera* m_plrCamera;
    WorldManager m_worldManager;

    std::shared_ptr<UIManager> m_uiManager;
};