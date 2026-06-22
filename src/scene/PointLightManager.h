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
        void addLight(const PointLight& light);
        void removeLight(const PointLight& light);
        void update(const Camera* camera);

        const std::vector<PointLight>& lights() const;
        uint8_t shadowLights() const;
        void setShadowLights(uint8_t count);

        void setMaxLights(uint32_t maxLights);
        uint32_t maxLights() const;

      private:
        gl::SSBO<PointLight> m_lights;
        uint8_t m_shadowLights = 8;
        uint32_t m_maxLights;
    };
}  // namespace engine