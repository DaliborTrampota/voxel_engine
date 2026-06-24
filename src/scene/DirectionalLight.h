#pragma once

#include <glm/glm.hpp>


namespace engine {

    class DirectionalLight {
      public:
        DirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity);

        void setDirection(const glm::vec3& direction);
        void setLightColor(const glm::vec3& color, float intensity);

        glm::vec3 direction() const { return m_direction; }
        glm::vec3 lightColor() const { return m_lightColor; }
        float lightIntensity() const { return m_lightIntensity; }

      private:
        glm::vec3 m_direction;
        glm::vec3 m_lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        float m_lightIntensity = 1.0f;
    };
}  // namespace engine