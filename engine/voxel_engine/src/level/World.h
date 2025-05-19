#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "Chunk.h"
#include "ITerrainGenerator.h"
#include "ThreadPool.h"

#include <shared_mutex>

namespace gl {
    class ShaderPipeline;
}

namespace engine {

    class Chunk;
    struct ChunkID;

    class World {
      public:
        World(std::unique_ptr<ITerrainGenerator> gen, uint32_t genThreads = 8);
        ~World();

        /// @brief Loads chunks in the given range.
        /// @param unloadRest If true, all chunks outside the view distance will be unloaded.
        void loadChunks(const glm::vec3& from, const glm::vec3& to, bool unloadRest = false);

        /// @brief Unloads chunks in the given range.
        void unloadChunks(const glm::vec3& from, const glm::vec3& to);

        /// @brief Unloads all chunks except the given ones.
        /// @param except Chunks to keep loaded.
        void unloadAllChunks(const std::vector<ChunkID>& except = {});

        bool checkBlock(glm::vec3 pos, Block& curBlock, glm::ivec3 dir) const;

        const std::unordered_set<ChunkID>& loadedChunks() const { return m_loadedChunks; }

      private:
        std::unordered_map<ChunkID, Chunk*> m_chunks;
        std::unordered_set<ChunkID> m_loadedChunks;
        std::unique_ptr<ITerrainGenerator> m_generator = nullptr;

        ThreadPool m_genPool;

        friend class Chunk;
        friend class Engine;

        void createChunk(ChunkID id, bool load);
    };

}  // namespace engine