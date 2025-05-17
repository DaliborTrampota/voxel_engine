#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <unordered_set>
#include <memory>

#include "ThreadPool.h"
#include "Chunk.h"
#include "ITerrainGenerator.h"

#include <shared_mutex>

namespace gl {
	class ShaderPipeline;
}

namespace engine {

	class Chunk;
	struct ChunkID;

	constexpr glm::ivec3 ChunkDim = glm::ivec3(16, 16, 16);
	int const TERRAIN_HEIGHT = 64;

	class World
	{
	public:
		World(uint32_t genThreads = 8);
		World(std::unique_ptr<ITerrainGenerator> gen, uint32_t genThreads = 8);
		~World();

        void generator(std::unique_ptr<ITerrainGenerator> gen);

		bool checkBlock(glm::vec3 pos, Block &curBlock, glm::ivec3 dir) const;

		void updateViewDistance(glm::vec3 pos);

        const std::unordered_set<ChunkID>& loadedChunks() const { return m_loadedChunks; }
	private:
		std::unordered_map<ChunkID, Chunk*> m_chunks;
		std::unordered_set<ChunkID> m_loadedChunks;
		std::unique_ptr<ITerrainGenerator> m_generator = nullptr;

		ThreadPool m_genPool;

		friend class Chunk;
        friend class Engine;
	};

}