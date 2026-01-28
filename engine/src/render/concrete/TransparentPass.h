#pragma once

#include "../RenderPass.h"


namespace engine {

    class TransparentPass : public RenderPass {
      public:
        explicit TransparentPass(glm::ivec2 resolution);

        void beforeRender(Engine& engine, uint8_t pass) override;
        void afterRender(Engine& engine, uint8_t pass) override;


        void init();
    };
}  // namespace engine