#pragma once

#include "../RenderPass.h"

#include <LWGL/buffer/FBO.h>
#include <LWGL/render/ShaderProgram.h>

namespace engine {
    class TonemapPass : public RenderPass {
      public:
        explicit TonemapPass(glm::ivec2 resolution)
            : RenderPass(resolution, RenderPass::Tonemap),
              m_tonemapShader(
                  "resources/shaders/utility/FullscreenQuad.vert",
                  "resources/shaders/Tonemap.frag",
                  "Tonemap"
              ) {
            init();
        }

        void beforeRender(Engine& engine, uint8_t pass) override;
        void afterRender(Engine& engine, uint8_t pass) override {}

        void resize(glm::ivec2 resolution) override;

      private:
        gl::ShaderProgram m_tonemapShader;

        void init();

        float m_exposure = 1.0f;
    };
}  // namespace engine