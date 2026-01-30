#pragma once

#include "../RenderPass.h"


namespace engine {

    class DirectionalShadowPass : public RenderPass {
      public:
        explicit DirectionalShadowPass(
            glm::ivec2 resolution,
            const Material* material,
            const gl::FBO* fbo,
            const glm::ivec2& shadowRes
        );

        void beforeRender(Engine& engine, uint8_t pass) override;
        void afterRender(Engine& engine, uint8_t pass) override;
    };
}  // namespace engine