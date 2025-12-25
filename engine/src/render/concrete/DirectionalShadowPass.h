#pragma once

#include "../RenderPass.h"


namespace engine {

    class DirectionalShadowPass : public RenderPass {
      public:
        void beforeRender(Engine& engine, uint8_t pass) override;
        void afterRender(Engine& engine, uint8_t pass) override;

        static std::unique_ptr<RenderPass> create(
            const Material* material, const gl::FBO* fbo, const glm::ivec2& resolution
        );

      protected:
        explicit DirectionalShadowPass(
            ID id, const Material* material, const gl::FBO* fbo, const glm::ivec2& resolution
        )
            : RenderPass(id) {
            this->material = material;
            this->fbo = fbo;
            viewportSize = resolution;
        }
    };
}  // namespace engine