#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>

#include "Chunk.h"
#include "ITerrainGenerator.h"
#include "ThreadPool.h"
#include "block/Vertex.h"
#include "render/Renderable.h"

#include <core/render/Material.h>

namespace gl {
    class ShaderPipeline;
}

namespace engine {
    static inline constexpr BlockID INVALID_BLOCK = -1;

    class Chunk;
    class Engine;
    struct ChunkID;
    struct RenderContext;

    class World : public Renderable {
      public:
        World(std::unique_ptr<ITerrainGenerator> gen, uint32_t genThreads = 8);
        ~World();

        /// @section Chunk management

        /// @brief Loads chunks in the given range.
        /// @param unloadRest If true, all chunks outside the view distance will be unloaded.
        void loadChunks(const glm::vec3& from, const glm::vec3& to, bool unloadRest = false);

        /// @brief Unloads chunks in the given range.
        void unloadChunks(const glm::vec3& from, const glm::vec3& to);

        /// @brief Unloads all chunks except the given ones.
        /// @param except Chunks to keep loaded.
        void unloadAllChunks(const std::vector<ChunkID>& except = {});


        /// @section Block management

        /// @brief Gets the block ID at the given position.
        /// @return the block ID or engine::INVALID_BLOCK if:
        ///         - The position is out of bounds (eg less than or greater than Chunk::Dims)
        ///         - The chunk is not generated
        BlockID getBlockID(const ChunkID& chID, const glm::ivec3& pos);

        bool checkBlock(glm::vec3 pos, Block& curBlock, glm::ivec3 dir) const;


        const std::unordered_set<ChunkID>& loadedChunks() const { return m_loadedChunks; }


        virtual void render(Engine& engine, int pass = 0) override;
        const gl::Material& getMaterial() const { return m_material; }

      protected:
        std::unordered_map<ChunkID, Chunk*> m_chunks;
        std::unordered_set<ChunkID> m_loadedChunks;
        std::unique_ptr<ITerrainGenerator> m_generator = nullptr;
        gl::Material m_material;

        ThreadPool m_genPool;

        friend class Chunk;
        friend class Engine;

      private:
        void createChunk(ChunkID id, bool load);
    };

}  // namespace engine