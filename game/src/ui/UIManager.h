#pragma once

#include <memory>

#include <input/GLFWEventSite.h>
#include <input/GLFWEvents.h>
#include <render/EngineEventSite.h>

#include <glm/glm.hpp>

#include <LWGL/render/ShaderProgram.h>
#include <UI/Renderer.h>

#include <scene/Updateable.h>

struct GLFWcursor;

namespace ui {
    class Renderer;
    class Label;
}  // namespace ui

class UIManager : public GLFWEventSite,
                  public EngineEventSite,
                  public engine::Updateable {
  public:
    UIManager(glm::ivec2 screenSize);
    ~UIManager() = default;

    void createUI();

    void update(float dt) override;
    void render();

  protected:
    void mouseMoveEvent(::MouseEvent* pEvent) override;
    void mouseButtonEvent(MouseButtonEvent* pEvent) override;
    void windowResizeEvent(ResizeEvent* ev) override;


  private:
    gl::ShaderProgram m_fboShader;
    ui::Renderer m_renderer;
    unsigned int m_vaoID = 0;
    unsigned int m_vboID = 0;

    glm::ivec2 m_lastMousePos = {-1, -1};

    std::shared_ptr<ui::Label> m_fpsLabel;
    int samples = 0;
    float fps = 0.f;
    int fpsMax = 0;
    int fpsMin = 10'000;


    inline static GLFWcursor* s_currentCursor = nullptr;
};