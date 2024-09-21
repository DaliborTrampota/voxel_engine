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

	const Face Face::TriangleFace(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3, uint32_t texID)
	{
		Face face;

		glm::vec3 n = glm::normalize(glm::cross(p2 - p1, p3 - p1));

		face.m_vertices.emplace_back(
			p1,
			n,
			uv1
		);

		face.m_vertices.emplace_back(
			p2,
			n,
			uv2
		);

		face.m_vertices.emplace_back(
			p3,
			n,
			uv3
		);

		face.setData(texID, 0);

		return face;
	}

	const Face Face::SquareFace(glm::vec3 start, glm::vec3 end, glm::vec3 n, glm::vec2 uvStart, glm::vec2 uvEnd, uint32_t texID)
	{
		glm::vec3 mid = (start + end) / 2.0f;
		glm::vec3 dir = glm::cross(n, end - mid);
		
		glm::vec3 corners[4] = {
			mid - dir,
			start,
			end,
			mid + dir
		};

		glm::vec2 uvs[4] = {
			(uvStart + glm::vec2(uvEnd.x, 0)),
			uvEnd,
			uvStart,
			(uvStart + glm::vec2(0, uvEnd.y)),
		};

		int indices[6] = {
			2, 1, 0,
			1, 2, 3
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

		
		face.setCull(n);
		face.setData(texID, 0);

		return face;
	}
	const Face Face::CircleFace(glm::vec3 center, float r, int segments, glm::vec3 n, glm::vec2 uvStart, glm::vec2 uvEnd, uint32_t texID)
	{

		Face face;

		float angleStep = glm::two_pi<float>() / segments;
		float angle = .0f;

		
		glm::vec3 axis1 = glm::normalize(glm::vec3(std::copysign(n.z, n.x), std::copysign(n.z, n.y), -std::copysign(std::abs(n.x) + std::abs(n.y), n.z))); // https://math.stackexchange.com/a/4112622
		glm::vec3 axis2 = glm::normalize(glm::cross(n, axis1));

		for (int i = 0; i < segments; ++i)
		{
			float nextAngle = angle + angleStep;

			glm::vec3 start = center + r * axis1 * glm::cos(angle) + r * axis2 * glm::sin(angle);
			glm::vec3 end = center + r * axis1 * glm::cos(nextAngle) + r * axis2 * glm::sin(nextAngle);

			face.m_vertices.emplace_back(
				center,
				n,
				glm::vec2(0.5f, 0.5f)
			);

			face.m_vertices.emplace_back(
				start,
				n,
				glm::vec2(0.5f + glm::cos(angle) / 2.0f, 0.5f + glm::sin(angle) / 2.0f)
			);

			face.m_vertices.emplace_back(
				end,
				n,
				glm::vec2(0.5f + glm::cos(nextAngle) / 2.0f, 0.5f + glm::sin(nextAngle) / 2.0f)
			);

			angle += angleStep;
		}

		face.setCull(n);
		face.setData(texID, 0);


		return face;
	}
	const Face Face::CylinderFace(glm::vec3 center, float r, float h, int segments, glm::vec3 n, glm::vec2 uvStart, glm::vec2 uvEnd, uint32_t texID)
	{
		Face face;

		float angleStep = glm::two_pi<float>() / segments;
		float angle = .0f;


		glm::vec3 axis1 = glm::normalize(glm::vec3(std::copysign(n.z, n.x), std::copysign(n.z, n.y), -std::copysign(std::abs(n.x) + std::abs(n.y), n.z))); // https://math.stackexchange.com/a/4112622
		glm::vec3 axis2 = glm::normalize(glm::cross(n, axis1));

		for (int i = 0; i < segments; ++i)
		{
			float nextAngle = angle + angleStep;

			glm::vec3 start = center + r * axis1 * glm::cos(angle) + r * axis2 * glm::sin(angle);
			glm::vec3 end = center + r * axis1 * glm::cos(nextAngle) + r * axis2 * glm::sin(nextAngle);

			glm::vec3 normal = glm::normalize(glm::cross(end - start, n));

			float uRange = uvEnd.x - uvStart.x;
			float u0 = angle / glm::two_pi<float>();
			float u1 = nextAngle / glm::two_pi<float>();

			glm::vec2 uvs[4] = {
				glm::vec2(uRange * u0 + uRange, uvStart.y),
				glm::vec2(uRange * u1 + uRange, uvStart.y),
				glm::vec2(uRange * u1 + uRange, uvEnd.y),
				glm::vec2(uRange * u0 + uRange, uvEnd.y)
			};

			face.m_vertices.emplace_back(
				start,
				normal,
				uvs[0]
			);

			face.m_vertices.emplace_back(
				end + n * h,
				normal,
				uvs[2]
			);

			face.m_vertices.emplace_back(
				start + n * h,
				normal,
				uvs[3]
			);


			face.m_vertices.emplace_back(
				end + n * h,
				normal,
				uvs[2]
			);

			face.m_vertices.emplace_back(
				start,
				normal,
				uvs[0]
			);

			face.m_vertices.emplace_back(
				end,
				normal,
				uvs[1]
			);




			angle += angleStep;
		}

		
		face.setData(texID, 0);

		return face;
	}
}