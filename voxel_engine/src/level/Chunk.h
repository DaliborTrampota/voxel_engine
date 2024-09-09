#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace lvl {

	using T = int;
	using VoxelData = std::vector<std::vector<std::vector<T>>>;

	class Chunk
	{

	public:
		Chunk(glm::ivec2 coords);
		//~Chunk();

		void populate();
		void generateMesh();

	private:

		VoxelData data;

	};

}