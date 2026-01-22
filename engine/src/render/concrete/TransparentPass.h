#pragma once

#include "../RenderPass.h"


namespace engine {

    class TransparentPass : public RenderPass {
      public:
        void beforeRender(Engine& engine, uint8_t pass) override;
        void afterRender(Engine& engine, uint8_t pass) override;

        static std::unique_ptr<RenderPass> create(glm::ivec2 resolution) {
            return std::make_unique<TransparentPass>(
                TransparentPass(resolution, RenderPass::SceneTransparent)
            );
        }

      protected:
        explicit TransparentPass(glm::ivec2 resolution, ID id) : RenderPass(resolution, id, 2) {}
    };
}  // namespace engine