#pragma once

#include "../RenderPass.h"

#include <LWGL/render/ShaderProgram.h>

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

      private:
        gl::ShaderProgram m_compositeShader;

        void init();
    };
}  // namespace engine