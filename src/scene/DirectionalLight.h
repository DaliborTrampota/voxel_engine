#pragma once

#include <glm/glm.hpp>


namespace engine {

    class DirectionalLight {
      public:
        DirectionalLight(
            const glm::vec3& direction = glm::vec3(-0.5f, -1.0f, -0.5f),
            const glm::vec3& color = glm::vec3(1.0f, 1.0f, 1.0f),
            float intensity = 1.0f,
            bool castShadows = true
        )
            : m_direction(glm::normalize(direction)),
              m_lightColor(color),
              m_lightIntensity(intensity),
              m_castShadows(castShadows) {}

        void setDirection(const glm::vec3& direction) { m_direction = glm::normalize(direction); }
        void setLightColor(const glm::vec3& color) { m_lightColor = color; }
        void setIntensity(float intensity) { m_lightIntensity = intensity; }
        void setCastShadows(bool castShadows) { m_castShadows = castShadows; }

        bool castShadows() const { return m_castShadows; }
        glm::vec3 direction() const { return m_direction; }
        glm::vec3 lightColor() const { return m_lightColor; }
        float lightIntensity() const { return m_lightIntensity; }

      private:
        bool m_castShadows = true;
        glm::vec3 m_direction;
        glm::vec3 m_lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        float m_lightIntensity = 1.0f;
    };
}  // namespace engine