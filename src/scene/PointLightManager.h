#pragma once

#include <LWGL/buffer/SSBO.h>

#include <cstdint>
#include <vector>

#include "PointLight.h"

namespace engine {
    class Camera;

    class PointLightManager {
      public:
        static constexpr PointLight::ID InvalidID = 0xFFFFFFFF;

        PointLightManager(uint32_t maxLights = 1024);

        void bindLights(uint32_t binding);

        uint32_t lightCount() const;
        uint32_t shadowLightCount() const;

        [[nodiscard]] PointLight::ID addLight(PointLight light);
        void removeLight(PointLight::ID id);
        void update(const Camera* camera);

        /// @brief Get a light by its ID.
        /// @warning The returned pointer is invalidated next frame due to distance sorting done in update().
        PointLight* light(PointLight::ID id);
        const std::vector<PointLight>& lights() const;

        uint8_t maxShadowLights() const;
        void setMaxShadowLights(uint8_t count);

        void setMaxLights(uint32_t maxLights);
        uint32_t maxLights() const;

      private:
        gl::SSBO<PointLight> m_lights;
        uint32_t m_shadowLightCount = 0;
        uint8_t m_maxShadowLights = 8;
        uint32_t m_maxLights;

        PointLight::ID m_nextID = 0;
    };
}  // namespace engine