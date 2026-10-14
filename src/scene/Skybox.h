#pragma once

#include <LWGL/buffer/Buffer.h>
#include <LWGL/texture/CubeMap.h>

#include "render/Material.h"
#include "render/Renderable.h"

namespace engine {

    class Skybox : public Renderable {
      public:
        struct Settings {
            std::string top;
            std::string bottom;
            std::string front;
            std::string back;
            std::string left;
            std::string right;
        };
        Skybox(
            const Settings& settings = {
                .top = "resources/skybox/top.jpg",
                .bottom = "resources/skybox/bottom.jpg",
                .front = "resources/skybox/front.jpg",
                .back = "resources/skybox/back.jpg",
                .left = "resources/skybox/left.jpg",
                .right = "resources/skybox/right.jpg",
            }
        );

        void load(const Settings& settings);

        void render(Engine& engine, const Camera* camera, int pass = 0) override;

        void setSkyIntensity(float intensity);
        float skyIntensity() const { return m_skyIntensity; };
        void setLightDiskAngularRadius(float degrees);
        void setLightDiskEnabled(bool enabled) { m_lightDiskEnabled = enabled; }
        bool lightDiskEnabled() const { return m_lightDiskEnabled; }
        // Multiplied with the directional light's color.
        void setLightDiskColor(const glm::vec3& color) { m_lightDiskColor = color; }
        glm::vec3 lightDiskColor() const { return m_lightDiskColor; }

      private:
        // gl::Attributes<SkyboxVertex> m_attributes;
        gl::Buffer<float> m_buffer;
        Material m_material;
        gl::CubeMap m_cubeMap{false};

        float m_skyIntensity = 1.0f;
        float m_lightDiskBoost = 20.0f;
        bool m_lightDiskEnabled = true;
        glm::vec3 m_lightDiskColor = glm::vec3(1.0f);
    };
}  // namespace engine