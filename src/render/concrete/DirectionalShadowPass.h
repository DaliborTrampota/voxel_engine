#pragma once

#include "../RenderPass.h"

#include <LWGL/buffer/FBO.h>
#include <LWGL/buffer/UBO.h>
#include <LWGL/texture/TextureArray.h>
#include <render/Material.h>

#include <vector>


namespace engine {

    class Camera;
    class DirectionalLight;

    class DirectionalShadowPass : public RenderPass {
      public:
        DirectionalShadowPass(glm::ivec2 resolution, const glm::ivec2& shadowRes);

        /// @brief Pass nullptr to stop drawing the cascades; the shadow maps stay allocated.
        void setSource(const DirectionalLight* light, const Camera* camera);

        void beforeRender(Engine& engine, uint8_t pass) override;
        void afterRender(Engine& engine, uint8_t pass) override;

        bool shouldRun() const override;

        struct Cascade {
            float farPlane;
            glm::mat4 lightSpace;
        };

        const std::vector<Cascade>& cascadeSplits() const { return m_cascadeSplits; }
        gl::TextureArray& cascadeShadowMaps() { return m_cascadeShadowMaps; }
        /// @brief Cascade light-space matrices, refreshed in beforeRender; bind it to draw into the cascades.
        gl::UBO& lightSpaceUBO() { return m_lightSpaceUBO; }

      private:
        gl::TextureArray m_cascadeShadowMaps;
        gl::FBO m_depthFBO;
        Material m_depthShader;
        gl::UBO m_lightSpaceUBO;

        const DirectionalLight* m_light = nullptr;
        const Camera* m_target = nullptr;

        std::vector<Cascade> m_cascadeSplits = {
            Cascade{16.0f, glm::mat4(1)},
            Cascade{64.0f, glm::mat4(1)},
            Cascade{128.0f, glm::mat4(1)},
            Cascade{256.0f, glm::mat4(1)}
        };


        void calculateLightSpaceMatrices();
    };
}  // namespace engine