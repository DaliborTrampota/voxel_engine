#pragma once

#include <glm/glm.hpp>

#include <LWGL/buffer/FBO.h>

#include "render/Material.h"
#include "render/RenderPass.h"
#include "scene/Updateable.h"


namespace engine {

    class Engine;
    class Camera;

    class Sun : public Updateable {
      public:
        static inline float DistanceFromTarget = 32.0f;
        Sun(glm::ivec2 resolution,
            const glm::vec3* targetPosition,
            const glm::vec3& direction = glm::vec3(0.0f, -1.0f, 0.0f));

        void setTargetPosition(const glm::vec3* position);
        void setDirection(const glm::vec3& direction);
        void setLightColor(const glm::vec3& color, float intensity);

        glm::vec3 lightPosition() const {
            return *m_targetPosition - m_direction * DistanceFromTarget;
        }
        glm::vec3 direction() const { return m_direction; }
        glm::vec3 lightColor() const { return m_lightColor; }
        float lightIntensity() const { return m_lightIntensity; }
        glm::ivec2 resolution() const { return m_resolution; }

        void update(float dt) override;

        glm::mat4 getLightSpaceTransform() const;
        unsigned shadowMapTexture() const;

        const gl::FBO* shadowFBO() const { return &m_depthFBO; }
        const Material* shadowMaterial() const { return &m_depthShader; }

      protected:
        glm::ivec2 m_resolution;

        const glm::vec3* m_targetPosition;
        glm::vec3 m_direction;

        glm::mat4 m_view;
        glm::mat4 m_projection;

        gl::FBO m_depthFBO;
        Material m_depthShader;

        glm::vec3 m_lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        float m_lightIntensity = 1.0f;
    };
}  // namespace engine