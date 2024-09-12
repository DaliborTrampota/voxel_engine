#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "Structures/Attribute.h"

namespace lvl {

	class World;

	using T = int;
	using VoxelData = std::vector<std::vector<std::vector<T>>>;

	class Chunk
	{

	public:
		Chunk(World* world, glm::ivec2 coords);
		//~Chunk();

		void populate();
		void generateMesh();

		/* @returns number of triangles to render */
		size_t prepareRender(ShaderPipeline* pipeline);

	private:

		glm::ivec2 m_coords;
		VoxelData m_data;
		World* m_world;

		Attribute m_vertexData;

	};

}