#pragma once

#include <memory>

#include <glm/glm.hpp>

#include <LWGL/render/Material.h>
#include <UI/Renderer.h>


namespace ui {
    class Renderer;
}

class UIManager {
  public:
    UIManager(glm::ivec2 screenSize);

    void render();


  private:
    gl::Material m_fboShader;
    ui::Renderer m_renderer;
    unsigned int m_vaoID = 0;
    unsigned int m_vboID = 0;
};