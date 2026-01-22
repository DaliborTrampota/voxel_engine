#pragma once

#include <glm/glm.hpp>

#include <LWGL/buffer/FBO.h>
#include <LWGL/buffer/UBO.h>
#include <LWGL/texture/TextureArray.h>

#include "render/Material.h"
#include "render/RenderPass.h"
#include "scene/Updateable.h"


namespace engine {

    class Engine;
    class Camera;

    class Sun : public Updateable {
      public:
        static inline float DistanceFromTarget = 512.0f;

        Sun(glm::ivec2 resolution,
            const Camera* target,
            const glm::vec3& direction = glm::vec3(0.0f, -1.0f, 0.0f));

        struct Cascade {
            float farPlane;
            glm::mat4 lightSpace;
        };

        void setTarget(const Camera* target);
        void setDirection(const glm::vec3& direction);
        void setLightColor(const glm::vec3& color, float intensity);

        glm::vec3 lightPosition(glm::vec3& outLookAtPos) const;
        glm::vec3 direction() const { return m_direction; }
        glm::vec3 lightColor() const { return m_lightColor; }
        float lightIntensity() const { return m_lightIntensity; }
        glm::ivec2 resolution() const { return m_resolution; }

        void update(float dt) override;

        const std::vector<Cascade>& cascadeSplits() const { return m_cascadeSplits; }
        gl::TextureRef cascadeShadowMaps() const;

        const gl::FBO* shadowFBO() const { return &m_depthFBO; }
        Material* shadowMaterial() { return &m_depthShader; }

      protected:
        std::vector<Cascade> m_cascadeSplits = {
            Cascade{16.0f, glm::mat4(1)},
            Cascade{64.0f, glm::mat4(1)},
            Cascade{128.0f, glm::mat4(1)},
            Cascade{256.0f, glm::mat4(1)}
        };
        glm::ivec2 m_resolution;

        const Camera* m_target;
        glm::vec3 m_direction;

        // glm::mat4 m_view;
        // glm::mat4 m_projection;
        // glm::vec3 m_position;

        gl::UBO m_lightSpaceUBO;
        gl::FBO m_depthFBO;
        Material m_depthShader;

        glm::vec3 m_lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        float m_lightIntensity = 1.0f;

        void calculateLightSpaceMatrices();
    };
}  // namespace engine