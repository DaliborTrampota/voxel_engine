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
#include "block/Vertex.h"
#include "render/Material.h"
#include "render/Renderable.h"
#include "scene/PointLightManager.h"
#include "scene/Skybox.h"
#include "scene/Sun.h"
#include "scene/Updateable.h"
#include "utility/ThreadPool.h"


#include <LWGL/indirect/IndirectBuffer.h>
#include <LWGL/indirect/VertexPool.h>

namespace gl {
    class ShaderPipeline;
}

namespace engine {
    class Chunk;
    class Engine;
    struct ChunkID;
    struct RenderContext;

    class World : public Renderable,
                  public Updateable,
                  public LevelEventSource {
      public:
        World(std::unique_ptr<ITerrainGenerator> gen, uint32_t genThreads = 8);
        World(std::unique_ptr<ITerrainGenerator> gen, glm::ivec3 chunkDims, uint32_t genThreads = 8);
        ~World();

        /// @brief Get the chunk dimensions for this world.
        /// @return The dimensions of chunks in this world.
        glm::ivec3 chunkDims() const { return m_chunkDims; }

        /// @section Chunk management

        /// @brief Loads chunks in the given range.
        /// @param unloadRest If true, all chunks outside the range will be unloaded.
        /// @return A shared_future that becomes ready when all chunks are loaded and generated
        virtual std::future<void> loadChunks(
            const glm::ivec3& from, const glm::ivec3& to, bool unloadRest = false
        );

        /// @brief Unloads chunks in the given range.
        /// @note This function fires a ChunkUnloadEvent for each chunk that is unloaded.
        virtual void unloadChunks(const glm::ivec3& from, const glm::ivec3& to);

        /// @brief Unloads all chunks except the given ones.
        /// @param except Chunks to keep loaded.
        /// @note This function fires a ChunkUnloadEvent for each chunk that is unloaded.
        void unloadAllChunks(const std::vector<ChunkID>& except = {});

        /// @brief Unloads given chunks from memory.
        /// @param ids Chunks to unload.
        /// @note This function does not fire any events.
        void unloadChunksFromMemory(const std::vector<ChunkID>& ids);

        std::shared_ptr<Chunk> getChunk(const ChunkID& id);
        std::shared_ptr<const Chunk> getChunk(const ChunkID& id) const;

        /// @brief Gets a chunk and casts it to the specified custom chunk type.
        /// @tparam T The custom chunk type (must inherit from Chunk).
        /// @param id The chunk ID to retrieve.
        /// @return A shared_ptr to the chunk cast to type T.
        /// @note This is a convenience method that performs a static_pointer_cast internally.
        ///       If the chunk doesn't exist, returns nullptr.
        /// @example auto customChunk = world->getChunkAs<MyCustomChunk>(chunkID);
        template <typename T>
        std::shared_ptr<T> getChunkAs(const ChunkID& id);

        /// @brief Gets a const chunk and casts it to the specified custom chunk type.
        /// @tparam T The custom chunk type (must inherit from Chunk).
        /// @param id The chunk ID to retrieve.
        /// @return A shared_ptr to the const chunk cast to type T.
        /// @note This is a convenience method that performs a static_pointer_cast internally.
        ///       If the chunk doesn't exist, returns nullptr.
        template <typename T>
        std::shared_ptr<const T> getChunkAs(const ChunkID& id) const;

        const std::unordered_set<ChunkID>& loadedChunks() const { return m_loadedChunks; }

        /// @brief Checks if the surrounding chunks need to be updated due to a block change.
        /// @param chID The chunk that was changed.
        /// @param pos The position of the block that was changed.
        void checkAndUpdateSurroundingChunks(const ChunkID& chID, const glm::ivec3& pos);

        /// @section Block management

        /// @brief Gets the block ID at the given position.
        /// @param chID The chunk to query the block from
        /// @param pos The position of the block within the chunk (0 to Chunk::Dims)
        /// @param fallbackToGenerator If true, the generator will be used to get the block ID if the chunk is not generated
        /// @param state [out] The state of the block. If nullptr, the state will not be returned.
        /// @return the block ID or engine::InvalidBlockID if:
        ///         - The position is out of bounds (eg less than or greater than Chunk::Dims)
        ///         - The chunk is not generated and fallbackToGenerator is false
        BlockID getBlockID(
            const ChunkID& chID,
            const glm::ivec3& pos,
            bool fallbackToGenerator,
            BlockState** state = nullptr
        );

        void setBlock(
            const ChunkID& chID,
            const glm::ivec3& pos,
            BlockID blockID,
            std::optional<BlockState> state = std::nullopt
        );
        void setBlock(const ChunkID& chID, const glm::ivec3& pos, MultiBlock&& multiBlock);
        MultiBlock* getMultiBlock(const ChunkID& chID, const glm::ivec3& pos);


        void setBlock(
            glm::ivec3 pos, BlockID blockID, std::optional<BlockState> state = std::nullopt
        );
        void setBlock(glm::ivec3 pos, MultiBlock&& multiBlock);
        MultiBlock* getMultiBlock(glm::ivec3 pos);

        /// @brief Gets the block ID at the given position.
        /// @param pos The position of the block in world space
        /// @param fallbackToGenerator If true, the generator will be used to get the block ID if the chunk is not generated
        /// @param state [out] The state of the block. If nullptr, the state will not be returned.
        /// @return the block ID or engine::InvalidBlockID if:
        ///         - The chunk is not generated and fallbackToGenerator is false
        BlockID getBlockID(glm::vec3 pos, bool fallbackToGenerator, BlockState** state = nullptr);

        /// @brief Gets the terrain generator for this world.
        /// @return Pointer to the terrain generator.
        const ITerrainGenerator* getGenerator() const { return m_generator.get(); }
        const Material& getMaterial() const { return m_material; }
        Skybox& getSkybox() { return m_skybox; }
        PointLightManager& getPointLightManager() { return m_pointLightManager; }

        void setOmniShadowMaps(gl::CubeMapArray& omniShadowMaps);

        void setSun(Sun&& sun) { m_sun = std::make_shared<Sun>(std::move(sun)); }
        std::weak_ptr<Sun> getSun() const { return m_sun; }

        virtual void render(Engine& engine, const Camera* camera, int pass = 0) override;
        virtual void update(float dt) override;


        /// @section Events


        virtual void afterBlockSet(
            const glm::ivec3& pos, BlockID blockID, BlockState* state = nullptr
        ) {}
        // virtual void multiBlockUpdated(const glm::ivec3& pos, MultiBlock* block) {};

      protected:
        std::unordered_map<ChunkID, std::shared_ptr<Chunk>> m_chunks;
        std::unordered_set<ChunkID> m_loadedChunks;
        std::unique_ptr<ITerrainGenerator> m_generator = nullptr;
        glm::ivec3 m_chunkDims;

        // TODO size dynamically? based on ViewDistance?
        static constexpr uint32_t m_poolCapacity = 4'000'000;
        gl::VertexPool<Vertex> m_opaquePool{m_poolCapacity}, m_transparentPool{m_poolCapacity};
        gl::IndirectBuffer m_opaqueBuffer, m_transparentBuffer, m_omniShadowBuffer;


        Material m_material;
        Skybox m_skybox;
        std::shared_ptr<Sun> m_sun;
        PointLightManager m_pointLightManager;

        ThreadPool m_genPool;

        /// @brief Factory method for creating chunks. Override this to provide custom Chunk subclasses.
        /// @param id The chunk coordinates to create the chunk at.
        /// @return A shared pointer to the newly created chunk.
        /// @note The default implementation creates a standard Chunk.
        ///       Custom World subclasses can override this to return custom Chunk types.
        virtual std::shared_ptr<Chunk> createChunk(const ChunkID& id);

        /// @brief Checks if the face of current block facing given direction can be seen and thus should be rendered.
        /// @param curBlock The current block.
        /// @param pos The position of the current block.
        /// @param dir The direction from which the face is being checked.
        /// @return true if the face can be seen (face should be rendered), false otherwise.
        bool canSeeFace(const Block& curBlock, glm::vec3 pos, glm::ivec3 dir) const;


        friend class Chunk;

      private:
        void updateChunk(ChunkID id);
        mutable std::mutex m_mutex;
    };

    template <typename T>
    std::shared_ptr<T> World::getChunkAs(const ChunkID& id) {
        static_assert(std::is_base_of<Chunk, T>::value, "T must inherit from Chunk");
        auto chunk = getChunk(id);
        return std::static_pointer_cast<T>(chunk);
    }

    template <typename T>
    std::shared_ptr<const T> World::getChunkAs(const ChunkID& id) const {
        static_assert(std::is_base_of<Chunk, T>::value, "T must inherit from Chunk");
        auto chunk = getChunk(id);
        return std::static_pointer_cast<const T>(chunk);
    }

}  // namespace engine