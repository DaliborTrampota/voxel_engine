#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>


namespace vert {


	struct Vertex
	{
		Vertex(glm::vec3 pos, glm::vec3 n, glm::vec2 uv, int textureID, int ao);

		//void addTo();

		glm::vec3 m_pos, m_normal;
		glm::vec2 m_uv;
		int m_data;
	};

}