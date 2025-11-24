#pragma once

#include <glm/glm.hpp>

#include <LWGL/buffer/FBO.h>
#include <LWGL/render/Material.h>

#include "render/Renderable.h"

namespace engine {

    class Engine;
    class Camera;

    class Sun : public Renderable {
      public:
        enum Type {
            Static,
            Moving
        };

        Sun(glm::ivec2 resolution);

        void render(Engine& engine, const Camera* camera, int pass) override;


      protected:
        glm::ivec2 m_resolution;

        glm::vec3 m_position;
        glm::vec3 m_direction;

        glm::mat4 m_view;
        glm::mat4 m_projection;

        gl::FBO m_depthFBO;
        gl::Material m_depthShader;
    };
}  // namespace engine