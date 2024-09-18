#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "data/Vertex.h"

namespace builder {

	struct Face
	{
		std::vector<data::Vertex> m_vertices;

		void translate(glm::vec3 t);
		void setData(int textureID, int ao);


		Face() = default;

		static const Face SquareFace(glm::vec3 start, glm::vec3 end, glm::vec3 n, glm::vec2 uvStart, glm::vec2 uvEnd);
		static const Face CircleFace(glm::vec3 center, float r, int segments, glm::vec3 n, glm::vec2 uvStart, glm::vec2 uvEnd);
	};


	union Faces
	{
		Face face;
		std::vector<Face> faces;
	};
}
