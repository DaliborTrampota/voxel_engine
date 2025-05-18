#pragma once

#include <glm/glm.hpp>
#include <core/gl/VertexLayout.h>

namespace engine {
	struct Vertex
	{
		Vertex(glm::vec3 pos, glm::vec3 n, glm::vec2 uv, int textureID, int ao);
		Vertex(glm::vec3 pos, glm::vec3 n, glm::vec2 uv);

		void translate(glm::vec3 t);
		void setData(int textureID, int ao);

		glm::vec3 m_pos, m_normal;
		glm::vec2 m_uv;
		/// Holds TextureID (0-10bits) and AO (11-12)
		unsigned int m_data;

        static gl::VertexLayout layout() {
            return {
                sizeof(Vertex), // stride
                {
                    { 0, gl::VertexAttribute::Float, 3, offsetof(Vertex, m_pos) },
                    { 1, gl::VertexAttribute::Float, 3, offsetof(Vertex, m_normal) },
                    { 2, gl::VertexAttribute::Float, 2, offsetof(Vertex, m_uv) },
                    { 3, gl::VertexAttribute::UInt, 1, offsetof(Vertex, m_data) }
                }
            };
        }
	};
}