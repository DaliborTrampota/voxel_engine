#pragma once

#include "../RenderPass.h"

#include <LWGL/buffer/FBO.h>
#include <LWGL/texture/Texture2D.h>

#include "render/Material.h"


namespace engine {

    class TransparentPass : public RenderPass {
      public:
        explicit TransparentPass(glm::ivec2 resolution);

        void beforeRender(Engine& engine, uint8_t pass) override;
        void afterRender(Engine& engine, uint8_t pass) override;

        gl::Texture2D* accumulation() { return &m_accumulation; }
        gl::Texture2D* revealage() { return &m_revealage; }

      private:
        gl::FBO m_OIT;
        gl::Texture2D m_accumulation;
        gl::Texture2D m_revealage;

        Material m_transparentMat;

        void init();
    };
}  // namespace engine