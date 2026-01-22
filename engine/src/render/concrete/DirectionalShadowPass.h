#pragma once

#include "../RenderPass.h"


namespace engine {

    class DirectionalShadowPass : public RenderPass {
      public:
        void beforeRender(Engine& engine, uint8_t pass) override;
        void afterRender(Engine& engine, uint8_t pass) override;

        static std::unique_ptr<RenderPass> create(
            glm::ivec2 resolution,
            const Material* material,
            const gl::FBO* fbo,
            const glm::ivec2& shadowRes
        );

      protected:
        explicit DirectionalShadowPass(
            glm::ivec2 resolution,
            ID id,
            const Material* material,
            const gl::FBO* fbo,
            const glm::ivec2& shadowRes
        )
            : RenderPass(resolution, id) {
            this->material = material;
            this->fbo = fbo;
            viewportSize = shadowRes;
        }
    };
}  // namespace engine