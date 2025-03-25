#include "Geometry.h"
#include "CubeGeometry.h"

#include <cassert>
#include <vector>

#include "data/VertexData.h"
#include "data/Vertex.h"

namespace builder {

	CubeGeometry::CubeGeometry(data::TexID texture) :
		m_textures{ texture, texture, texture, texture, texture, texture }
	{
		//std::copy(data::f_faces.begin(), data::f_faces.end(), m_faces.begin());
		m_faces = data::f_faces;
	}

	CubeGeometry::CubeGeometry(data::TexID textures[6]) :
		m_textures{ textures[0], textures[1], textures[2], textures[3], textures[4], textures[5] }
	{
		m_faces = data::f_faces;
		//std::copy(data::f_faces.begin(), data::f_faces.end(), &m_faces);
	}

	bool CubeGeometry::sideSpecific()
	{
		return true;
	}

	data::TexID CubeGeometry::getTexture(int side) const
	{
		assert(side < 6);// , "Side must be less than 6");
		return m_textures[side];
	}

	Face CubeGeometry::getFace(int side) const
	{
		assert(side < 6);// , "Side must be less than 6");
		return m_faces[side];
	}

			

	void CubeGeometry::build(float scale, glm::vec3 offset, bool scaleUVs)
	{
	/*	float uvScale = scaleUVs ? scale : 1.0f;
		for (int side = 0; side < 6; ++side)
		{
			float vertices[18]{}, uvs[12]{};
			for (int i = 0; i < 6; ++i)
			{
				vertices[i * 3 + 0] = data::vertices[data::faces[side][i]][0] * scale + offset.x;
				vertices[i * 3 + 1] = data::vertices[data::faces[side][i]][0] * scale + offset.y;
				vertices[i * 3 + 2] = data::vertices[data::faces[side][i]][0] * scale + offset.z;

				uvs[i * 2 + 0] = data::uvs[i][0] * uvScale + (scaleUVs ? offset[data::axisForSide[side][0]] : 0);
				uvs[i * 2 + 0] = data::uvs[i][1] * uvScale + (scaleUVs ? offset[data::axisForSide[side][1]] : 0);
			}
			m_faces[side] = Face(vertices, uvs);
		}*/

		//for (int side = 0; side < 6; ++side) {
		//	m_faces[side] = Face::SquareFace(
		//		data::vertices[data::faces[side]]
		//		data::normals[side],
		//		glm::vec2{ 0, 0 },
		//		glm::vec2{ 1, 1 }
		//	);
		//}
	}

}