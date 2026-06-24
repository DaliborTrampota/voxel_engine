#pragma once

#include <LWGL/buffer/SSBO.h>

#include "PointLight.h"

namespace engine {
    class Camera;

    class PointLightManager {
      public:
        PointLightManager(uint32_t maxLights = 1024);

        void bindLights(uint32_t binding);

        uint32_t lightCount() const;
        uint32_t shadowLightCount() const;
        void addLight(const PointLight& light);
        void removeLight(const PointLight& light);
        void update(const Camera* camera);

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
    };
}  // namespace engine