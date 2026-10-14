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
                  "resources/shaders/color/Tonemap.frag",
                  "Tonemap"
              ) {
            init();
        }

        void beforeRender(Engine& engine, uint8_t pass) override;
        void afterRender(Engine& engine, uint8_t pass) override {}

        float exposure() const { return m_exposure; }
        void setExposure(float exposure) { m_exposure = exposure; }
        int getMode() const { return m_mode; }
        void setMode(int mode) { m_mode = mode; }

      private:
        gl::ShaderProgram m_tonemapShader;

        void init();

        float m_exposure = 1.0f;
        int m_mode = 1; // AgX default
    };
}  // namespace engine