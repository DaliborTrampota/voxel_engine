#pragma once

#include <LWGL/buffer/VertexLayout.h>
#include <glm/glm.hpp>

namespace engine {
    struct Vertex {
        Vertex(glm::vec3 pos, glm::vec3 n, glm::vec2 uv, int textureID, int ao);
        Vertex(glm::vec3 pos, glm::vec3 n, glm::vec2 uv);

        void translate(glm::vec3 t);
        void rotate(glm::vec3 axis, float angle);
        void data(int textureID, int ao);

        glm::vec3 pos, normal;
        glm::vec2 uv;
        /// Holds TextureID (0-15bits) and AO (16-17)
        unsigned int m_data;

        static gl::VertexLayout layout() {
            return {
                sizeof(Vertex),  // stride
                {
                    {0, gl::VertexAttribute::Float, 3, offsetof(Vertex, pos)},
                    {1, gl::VertexAttribute::Float, 3, offsetof(Vertex, normal)},
                    {2, gl::VertexAttribute::Float, 2, offsetof(Vertex, uv)},
                    {3, gl::VertexAttribute::UInt, 1, offsetof(Vertex, m_data)},
                },
            };
        }
    };
}  // namespace engine