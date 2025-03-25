#pragma once

#include <glm/glm.hpp>

#include <vector>
#include <array>

#include "../Block.h"
#include "Geometry.h"
#include "Face.h"

namespace data {
	struct Vertex;
}

namespace builder {

	class CubeGeometry : public Geometry
	{
	public:
		CubeGeometry(data::TexID texture);
		CubeGeometry(data::TexID textures[6]);

		bool sideSpecific();// TODO? override;

		Face getFace(int side) const;
		data::TexID getTexture(int side) const;

		void build(float scale, glm::vec3 offset, bool scaleUVs = false);

	private:
				
		std::array<Face, 6> m_faces;
		data::TexID m_textures[6];
	};
}