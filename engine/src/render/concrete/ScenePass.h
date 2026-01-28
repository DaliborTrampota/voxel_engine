#pragma once

#include "../RenderPass.h"

#include <LWGL/buffer/FBO.h>
#include <LWGL/texture/Texture2D.h>


namespace engine {

    class ScenePass : public RenderPass {
      public:
        explicit ScenePass(glm::ivec2 resolution) : RenderPass(resolution, RenderPass::Scene) {
            init();
        }

        void beforeRender(Engine& engine, uint8_t pass) override;
        void afterRender(Engine& engine, uint8_t pass) override;

        // ScenePass(const ScenePass&) = delete;
        // ScenePass& operator=(const ScenePass&) = delete;
        // ScenePass(ScenePass&&);
        // ScenePass& operator=(ScenePass&&) = delete;

        gl::FBO* FBO() { return &m_sceneFBO; }

        gl::Texture2D* depthMap() { return &m_depthMap; }
        gl::Texture2D* color() { return &m_opaque; }

      protected:
      private:
        gl::FBO m_sceneFBO;
        gl::Texture2D m_opaque;
        gl::Texture2D m_depthMap;

        void init();
    };
}  // namespace engine