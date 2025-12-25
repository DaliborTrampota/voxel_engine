#pragma once

#include "../RenderPass.h"


namespace engine {

    class ScenePass : public RenderPass {
      public:
        void beforeRender(Engine& engine, uint8_t pass) override {};
        void afterRender(Engine& engine, uint8_t pass) override {};

        static std::unique_ptr<RenderPass> create() {
            return std::make_unique<ScenePass>(ScenePass(RenderPass::Scene));
        }

      protected:
        explicit ScenePass(ID id) : RenderPass(id) {}
    };
}  // namespace engine