#pragma once

#include <future>
#include <glm/glm.hpp>
#include <memory>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <unordered_set>

#include "events/LevelEventSource.h"

#include "Chunk.h"
#include "ITerrainGenerator.h"
#include "render/Material.h"
#include "render/Renderable.h"
#include "scene/Skybox.h"
#include "scene/Updateable.h"
#include "utility/ThreadPool.h"


namespace gl {
    class ShaderPipeline;
}

namespace engine {
    static inline constexpr BlockID INVALID_BLOCK = -1;

    class Chunk;
    class Engine;
    struct ChunkID;
    struct RenderContext;

    class World : public Renderable,
                  public Updateable,
                  public LevelEventSource {
      public:
        World(std::unique_ptr<ITerrainGenerator> gen, uint32_t genThreads = 8);
        ~World();

        /// @section Chunk management

        /// @brief Loads chunks in the given range.
        /// @param unloadRest If true, all chunks outside the range will be unloaded.
        /// @return A shared_future that becomes ready when all chunks are loaded and generated
        std::future<void> loadChunks(
            const glm::ivec3& from, const glm::ivec3& to, bool unloadRest = false
        );

        /// @brief Unloads chunks in the given range.
        /// @note This function fires a ChunkUnloadEvent for each chunk that is unloaded.
        void unloadChunks(const glm::ivec3& from, const glm::ivec3& to);

        /// @brief Unloads all chunks except the given ones.
        /// @param except Chunks to keep loaded.
        /// @note This function fires a ChunkUnloadEvent for each chunk that is unloaded.
        void unloadAllChunks(const std::vector<ChunkID>& except = {});

        /// @brief Unloads given chunks from memory.
        /// @param ids Chunks to unload.
        /// @note This function does not fire any events.
        void unloadChunksFromMemory(const std::vector<ChunkID>& ids);

        Chunk* getChunk(const ChunkID& id);
        const Chunk* getChunk(const ChunkID& id) const;
        const std::unordered_set<ChunkID>& loadedChunks() const { return m_loadedChunks; }

        /// @brief Checks if the surrounding chunks need to be updated due to a block change.
        /// @param chID The chunk that was changed.
        /// @param pos The position of the block that was changed.
        void checkAndUpdateSurroundingChunks(const ChunkID& chID, const glm::ivec3& pos);

        /// @section Block management

        /// @brief Gets the block ID at the given position.
        /// @param chID The chunk to query the block from
        /// @param pos The position of the block within the chunk (0 to Chunk::Dims)
        /// @param state [out] The state of the block
        /// @param fallbackToGenerator If true, the generator will be used to get the block ID if the chunk is not generated
        /// @return the block ID or engine::INVALID_BLOCK if:
        ///         - The position is out of bounds (eg less than or greater than Chunk::Dims)
        ///         - The chunk is not generated
        BlockID getBlockID(
            const ChunkID& chID, const glm::ivec3& pos, BlockState*& state, bool fallbackToGenerator
        );

        /// @brief Gets the block ID at the given position.
        /// @param pos The position of the block in world space
        /// @param state [out] The state of the block
        /// @param fallbackToGenerator If true, the generator will be used to get the block ID if the chunk is not generated
        /// @return the block ID or engine::INVALID_BLOCK if:
        ///         - The position is out of bounds (eg less than or greater than Chunk::Dims)
        ///         - The chunk is not generated
        BlockID getBlockID(glm::vec3 pos, BlockState*& state, bool fallbackToGenerator);

        /// @brief Checks if the face of current block facing given direction can be seen and thus should be rendered.
        /// @param curBlock The current block.
        /// @param pos The position of the current block.
        /// @param dir The direction from which the face is being checked.
        /// @return true if the face can be seen (face should be rendered), false otherwise.
        bool canSeeFace(const Block& curBlock, glm::vec3 pos, glm::ivec3 dir) const;


        void setBlock(
            const ChunkID& chID,
            const glm::ivec3& pos,
            BlockID blockID,
            std::optional<BlockState> state = std::nullopt
        );
        void setBlock(
            glm::ivec3 pos, BlockID blockID, std::optional<BlockState> state = std::nullopt
        );

        void setBlock(const ChunkID& chID, const glm::ivec3& pos, MultiBlock&& multiBlock);
        void setBlock(glm::ivec3 pos, MultiBlock&& multiBlock);

        MultiBlock* getMultiBlock(const ChunkID& chID, const glm::ivec3& pos);
        MultiBlock* getMultiBlock(glm::ivec3 pos);


        virtual void render(Engine& engine, const Camera* camera, int pass = 0) override;
        virtual void update(float dt) override;

        const Material& getMaterial() const { return m_material; }
        Skybox& getSkybox() { return m_skybox; }

      protected:
        std::unordered_map<ChunkID, std::unique_ptr<Chunk>> m_chunks;
        std::unordered_set<ChunkID> m_loadedChunks;
        std::unique_ptr<ITerrainGenerator> m_generator = nullptr;

        Material m_material;
        Skybox m_skybox;

        ThreadPool m_genPool;

        friend class Chunk;
        friend class Engine;

      private:
        void updateChunk(ChunkID id);
        std::mutex m_mutex;
    };

}  // namespace engine