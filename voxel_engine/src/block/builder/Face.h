#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <optional>

#include "data/Vertex.h"

namespace builder {

	struct Face
	{
		std::vector<data::Vertex> m_vertices;
		glm::ivec3 m_cullDir;
		bool m_cull = false;

		void translate(glm::vec3 t);
		void setData(int textureID, int ao);

		void setCull(glm::ivec3 dir) {
			m_cull = true;
			m_cullDir = dir;
		}

		Face() = default;

		static const Face TriangleFace(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3, uint32_t texID);
		static const Face SquareFace(glm::vec3 start, glm::vec3 end, glm::vec3 n, glm::vec2 uvStart, glm::vec2 uvEnd, uint32_t texID);
		static const Face CircleFace(glm::vec3 center, float r, int segments, glm::vec3 n, glm::vec2 uvStart, glm::vec2 uvEnd, uint32_t texID);
		static const Face CylinderFace(glm::vec3 center, float r, float h, int segments, glm::vec3 n, glm::vec2 uvStart, glm::vec2 uvEnd, uint32_t texID);
	};

}
