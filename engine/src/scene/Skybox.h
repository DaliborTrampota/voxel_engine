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

      private:
        // gl::Attributes<SkyboxVertex> m_attributes;
        gl::Buffer<float> m_buffer;
        Material m_material;
        gl::CubeMap m_cubeMap{false};
    };
}  // namespace engine