#pragma once

#include <glm/glm.hpp>
#include <unordered_map>

#include "Chunk.h"

class TerrainGenerator;
class ShaderPipeline;

namespace lvl {

	class Chunk;
	struct ChunkID;

	constexpr glm::ivec3 ChunkDim = glm::ivec3(16, 16, 16);
	int const TERRAIN_HEIGHT = 64;

	class World
	{
	public:
		World() = default;
		World(TerrainGenerator* gen);
		~World();


		bool checkBlock(glm::vec3 pos, data::Block &curBlock, glm::ivec3 dir) const;


		void render(ShaderPipeline* pipeline);

	private:
		unsigned int m_VAO;



		std::unordered_map<ChunkID, Chunk*> chunks;
		TerrainGenerator* generator;


		friend class Chunk;
	};

}