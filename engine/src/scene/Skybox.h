#pragma once

#include <LWGL/buffer/Buffer.h>
#include <LWGL/texture/CubeMap.h>

#include "render/Material.h"
#include "render/Renderable.h"

namespace engine {

    class Skybox : public gl::CubeMap,
                   public Renderable {
      public:
        Skybox(unsigned int unit);

        void render(Engine& engine, const Camera* camera, int pass = 0) override;

      private:
        // gl::Attributes<SkyboxVertex> m_attributes;
        gl::Buffer<float> m_buffer;
        Material m_material;
    };
}  // namespace engine