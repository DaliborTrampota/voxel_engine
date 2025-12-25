#pragma once

#include "../RenderPass.h"


namespace engine {

    class TransparentPass : public RenderPass {
      public:
        void beforeRender(Engine& engine, uint8_t pass) override;
        void afterRender(Engine& engine, uint8_t pass) override;

        static std::unique_ptr<RenderPass> create() {
            return std::make_unique<TransparentPass>(TransparentPass(RenderPass::SceneTransparent));
        }

      protected:
        explicit TransparentPass(ID id) : RenderPass(id, 2) {}
    };
}  // namespace engine