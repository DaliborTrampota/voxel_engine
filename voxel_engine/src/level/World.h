#pragma once

#include <glm/glm.hpp>

#include <unordered_map>

#include "Chunk.h"
#include "TerrainGenerator.h"

struct KeyFuncs
{
	size_t operator()(const glm::ivec2& k)const
	{
		return std::hash<int>()(k.x) ^ std::hash<int>()(k.y << 1);
	}

	bool operator()(const glm::ivec2& a, const glm::ivec2& b)const
	{
		return a.x == b.x && a.y == b.y;
	}
};


namespace lvl {

	int const CHUNK_SIZE = 16;
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

		std::unordered_map<glm::ivec2, Chunk*, KeyFuncs, KeyFuncs> chunks;
		TerrainGenerator* generator;


		friend class Chunk;
	};

}
