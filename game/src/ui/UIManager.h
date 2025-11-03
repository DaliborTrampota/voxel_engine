#pragma once

#include <memory>

#include <LWGL/events/GLEventSite.h>
#include <LWGL/events/GLEvents.h>

#include <glm/glm.hpp>

#include <LWGL/render/Material.h>
#include <UI/Renderer.h>

struct GLFWcursor;

namespace ui {
    class Renderer;
}

class UIManager : public GLEventSite {
  public:
    UIManager(glm::ivec2 screenSize);
    ~UIManager() = default;

    void render();

  protected:
    void mouseMoveEvent(::MouseEvent* pEvent) override;
    void mouseButtonEvent(::MouseButtonEvent* pEvent) override;


  private:
    gl::Material m_fboShader;
    ui::Renderer m_renderer;
    unsigned int m_vaoID = 0;
    unsigned int m_vboID = 0;

    glm::ivec2 m_lastMousePos = {-1, -1};

    inline static GLFWcursor* s_currentCursor = nullptr;
};