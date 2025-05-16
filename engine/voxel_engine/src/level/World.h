#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <unordered_set>

#include "ThreadPool.h"
#include "Chunk.h"

#include <shared_mutex>

namespace gl {
	class ShaderPipeline;
}

namespace engine {

	class TerrainGenerator;
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


		bool checkBlock(glm::vec3 pos, Block &curBlock, glm::ivec3 dir) const;

		void updateViewDistance(glm::vec3 pos);

        const std::unordered_set<ChunkID>& loadedChunks() const { return m_loadedChunks; }
	private:
		mutable std::shared_mutex m_chunksMutex;
		std::unordered_map<ChunkID, Chunk*> m_chunks;
		std::unordered_set<ChunkID> m_loadedChunks;
		TerrainGenerator* m_generator;

		ThreadPool<>* m_genPool = nullptr;

		friend class Chunk;
        friend class Engine;
	};

}