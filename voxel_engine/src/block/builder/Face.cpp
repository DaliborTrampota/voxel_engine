#include "Face.h"

#include <glm/gtc/constants.hpp>

#include "data/VertexData.h"

namespace builder {

	void Face::translate(glm::vec3 t)
	{
		for (auto& v : m_vertices)
			v.m_pos += t;
	}

	void Face::setData(int textureID, int ao)
	{
		for (auto& v : m_vertices)
			v.setData(textureID, ao);
	}

	const Face Face::SquareFace(glm::vec3 start, glm::vec3 end, glm::vec3 n, glm::vec2 uvStart, glm::vec2 uvEnd)
	{
		glm::vec3 mid = (start + end) / 2.0f;
		glm::vec3 dir = glm::cross(n, end - mid);
		
		glm::vec3 corners[4] = {
			mid + dir,
			start,
			end,
			mid - dir
		};

		glm::vec2 uvs[4] = {
			(uvStart + glm::vec2(0, uvEnd.y)),
			uvStart,
			uvEnd,
			(uvStart + glm::vec2(uvEnd.x, 0))
		};

		int indices[6] = {
			0, 1, 2,
			3, 2, 1
		};

		Face face;

		for (int i = 0; i < 6; ++i)
		{
			face.m_vertices.emplace_back(
				corners[indices[i]],
				n,
				uvs[indices[i]]
			);
		}

		return face;
	}
	const Face Face::CircleFace(glm::vec3 center, float r, int segments, glm::vec3 n, glm::vec2 uvStart, glm::vec2 uvEnd)
	{

		Face face;

		float angleStep = glm::two_pi<float>() / segments;
		float angle = .0f;

		for (int i = 0; i < segments; ++i)
		{
			float nextAngle = angle + angleStep;

			glm::vec3 start = center + glm::vec3{ r * glm::cos(angle), r * glm::sin(angle), 0 };
			glm::vec3 end = center + glm::vec3{ r * glm::cos(nextAngle), r * glm::sin(nextAngle), 0 };

			angle += angleStep;

			face.m_vertices.emplace_back(
				center,
				n,
				(center + glm::vec3{ glm::cos(angle), glm::sin(angle), 0 }) / 2.0f
			);

			face.m_vertices.emplace_back(
				start,
				n,
				(center + glm::vec3{ glm::cos(angle), glm::sin(angle), 0 }) / 2.0f
			);

			face.m_vertices.emplace_back(
				end,
				n,
				(center + glm::vec3{ glm::cos(nextAngle), glm::sin(nextAngle), 0 }) / 2.0f
			);
		}


		return Face();
	}
}