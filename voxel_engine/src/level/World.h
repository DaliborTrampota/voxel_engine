#pragma once

#include <glm/glm.hpp>

#include <unordered_map>

#include "Chunk.h"
#include "TerrainGenerator.h"


namespace lvl {

	constexpr glm::ivec3 ChunkDim = glm::ivec3(16, 16, 16);
	int const TERRAIN_HEIGHT = 64;

	class World
	{
	public:
		World() = default;
		World(TerrainGenerator* gen);
		~World();

		void render(ShaderPipeline* pipeline);

	private:
		unsigned int m_VAO;

		std::unordered_map<ChunkID, Chunk*> chunks;
		TerrainGenerator* generator;


		friend class Chunk;
	};

}
