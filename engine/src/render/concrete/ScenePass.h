#pragma once

#include "../RenderPass.h"


namespace engine {

    class ScenePass : public RenderPass {
      public:
        void beforeRender(Engine& engine, uint8_t pass) override {};
        void afterRender(Engine& engine, uint8_t pass) override {};

        static std::unique_ptr<RenderPass> create(glm::ivec2 resolution) {
            return std::make_unique<ScenePass>(ScenePass(resolution, RenderPass::Scene));
        }

      protected:
        explicit ScenePass(glm::ivec2 resolution, ID id) : RenderPass(resolution, id) {}
    };
}  // namespace engine