#pragma once

#include <LWGL/buffer/Buffer.h>
#include <LWGL/render/Material.h>
#include <LWGL/texture/CubeMap.h>

#include "render/Renderable.h"

namespace engine {

    // struct SkyboxVertex {
    //     glm::vec3 position;
    //     static gl::VertexLayout layout() {
    //         return {
    //             .stride = sizeof(SkyboxVertex),
    //             .attributes = {
    //                 {0, gl::VertexAttribute::Type::Float, 3, offsetof(SkyboxVertex, position)}
    //             }
    //         };
    //     };
    // };

    class Skybox : public gl::CubeMap,
                   public Renderable {
      public:
        Skybox(unsigned int unit);

        void render(Engine& engine, const Camera* camera, int pass = 0) override;

      private:
        // gl::Attributes<SkyboxVertex> m_attributes;
        gl::Buffer<float> m_buffer;
        gl::Material m_material;
    };
}  // namespace engine