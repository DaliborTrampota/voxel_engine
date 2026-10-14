#pragma once

#include "../RenderPass.h"

#include <LWGL/buffer/FBO.h>
#include <LWGL/render/ShaderProgram.h>
#include <LWGL/texture/Texture2D.h>


namespace engine {

    // TODO this should be private pass always at the end of the pipeline?
    class CompositePass : public RenderPass {
      public:
        CompositePass(glm::ivec2 resolution)
            : RenderPass(resolution, RenderPass::Composite),
              m_compositeShader(
                  "resources/shaders/utility/FullscreenQuad.vert",
                  "resources/shaders/CompositeFrag.glsl",
                  "Composite"
              ) {
            init();
        }
        void beforeRender(Engine& engine, uint8_t pass) override;
        void afterRender(Engine& engine, uint8_t pass) override {}

        gl::Texture2D* color() { return &m_hdrTexture; }

        void resize(glm::ivec2 resolution) override;

      private:
        gl::ShaderProgram m_compositeShader;
        gl::FBO m_hdrFBO;
        gl::Texture2D m_hdrTexture{false};

        void init();
    };
}  // namespace engine