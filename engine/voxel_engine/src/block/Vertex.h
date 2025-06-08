#pragma once

#include <core/gl/geometry/VertexLayout.h>
#include <glm/glm.hpp>

namespace engine {
    struct Vertex {
        Vertex(glm::vec3 pos, glm::vec3 n, glm::vec2 uv, int textureID, int ao);
        Vertex(glm::vec3 pos, glm::vec3 n, glm::vec2 uv);

        void translate(glm::vec3 t);
        void data(int textureID, int ao);

        glm::vec3 pos, normal;
        glm::vec2 uv;
        /// Holds TextureID (0-10bits) and AO (11-12)
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