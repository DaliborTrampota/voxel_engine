#pragma once

#include "../RenderPass.h"
#include "render/Material.h"
#include "scene/PointLightManager.h"

#include <LWGL/buffer/FBO.h>
#include <LWGL/texture/CubeMapArray.h>

#include <glm/glm.hpp>

namespace engine {

    class PointLightShadowPass : public RenderPass {
      public:
        explicit PointLightShadowPass(
            glm::ivec2 resolution, PointLightManager* pointLightManager, const glm::ivec2& shadowRes
        );

        void beforeRender(Engine& engine, uint8_t pass) override;
        void afterRender(Engine& engine, uint8_t pass) override;

        uint8_t passes() const override;
        bool shouldRun() const override;

        gl::CubeMapArray& shadowMaps() { return m_shadowMaps; }

      private:
        gl::CubeMapArray m_shadowMaps;
        gl::FBO m_shadowFBO;
        Material m_material;
        PointLightManager* m_pointLightManager;
    };
}  // namespace engine