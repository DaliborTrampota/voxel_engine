#pragma once

#include <array>
#include <atomic>
#include <glm/glm.hpp>
#include <iostream>
#include <mutex>


#include "../hash.h"
#include "ChunkID.h"

#include "LayerBuffer.h"
#include "block/Block.h"
#include "block/BlockState.h"
#include "block/Neighbours.h"
#include "block/VariantBlock.h"
#include "block/Vertex.h"
#include "data/IChunkData.h"


#include <LWGL/buffer/Attributes.h>
#include <LWGL/indirect/IndirectTypes.h>

namespace gl {
    class ShaderPipeline;
}

namespace engine {

    class Block;
    class VariantBlock;
    class MultiBlock;

    class World;
    class Engine;
    struct RenderContext;


    class Chunk : public ISerializable {
      public:
        static inline glm::ivec3 Dims{16, 16, 16};

        Chunk(World* world, ChunkID coords, std::unique_ptr<IChunkData> data);
        Chunk(Chunk&) = delete;
        Chunk(Chunk&&) = delete;
        ~Chunk();

        /// @return ID or coordinates of the chunk in the world.
        const ChunkID& id() const { return m_coords; }
        /// @return Position of the chunk in world space.
        glm::ivec3 position() const { return m_coords * m_data->dims; }

        /// @brief Get the dimensions of this chunk type.
        /// @return The dimensions (x, y, z) of the chunk.
        /// @note Override this in derived classes to provide custom chunk dimensions.
        virtual glm::ivec3 dims() const { return m_data->dims; }

        World* world() const { return m_world; }

        template <typename T>
        T* worldAs() const {  // The user knows what world the chunk belongs to, thus static_cast
            return static_cast<T*>(m_world);
        }

        /// @brief Generates the chunk data per TerrainGenerator if not generated yet.
        void populateTerrainData();
        bool generated() const { return m_generated; }

        /// @brief Generates the mesh data for the chunk.
        /// @return true if the mesh was generated, false if it was already generating.
        /// @note This function is thread-safe.
        bool generateMesh();


        const Block* getBlock(glm::ivec3 pos, BlockState** state = nullptr);
        void setBlock(const glm::ivec3& pos, BlockID block);
        void setBlock(const glm::ivec3& pos, BlockID block, BlockState state);
        void setBlock(const glm::ivec3& pos, MultiBlock&& multiBlock);


        /// @return ChunkData structure containing all the block/terrain data.
        IChunkData* data() { return m_data.get(); }
        /// @return const ChunkData structure containing all the block/terrain data.
        const IChunkData* data() const { return m_data.get(); }

        void uploadVertices(gl::VertexPool<Vertex>& opaque, gl::VertexPool<Vertex>& transparent);
        void releaseVertices(gl::VertexPool<Vertex>& opaque, gl::VertexPool<Vertex>& transparent);

        glm::mat4 modelMatrix() const;
        const gl::PoolAllocation& transparentAlloc() const;
        const gl::PoolAllocation& opaqueAlloc() const;

        Neighbours getNeighbouringBlocks(glm::ivec3 pos) const;

        /// @section Events
        // TODO probably remove this
        virtual void afterGenerated() {}


        void serialize(std::ostream& out) const override;
        void deserialize(std::istream& in) override;


      protected:
        World* m_world;
        ChunkID m_coords;
        std::unique_ptr<IChunkData> m_data;
        std::atomic_bool m_dirty{false};

        struct GeometryState {
            glm::vec3 axis;
            float angle;
        };

        struct MeshGenContext {
            const Block* block;
            const Geometry* geometry;
            GeometryState geometryState;
            LayerData& storage;
            glm::ivec3 posInChunk;
            glm::ivec3 worldPos;
        };

        GeometryState calculateGeometryState(const Block* block, const BlockState* state) const;
        void generateMeshForGeometry(const MeshGenContext& ctx);

        void generateMeshForBlock(
            const Block* block,
            glm::ivec3 pos,
            const BlockState* state,
            const glm::ivec3& chunkBlockCoords
        );
        void generateMeshForBlock(
            const VariantBlock* block,
            glm::ivec3 pos,
            const BlockState* state,
            const glm::ivec3& chunkBlockCoords
        );
        void generateMeshForBlock(
            const MultiBlock* block,
            glm::ivec3 pos,
            const BlockState* state,
            const glm::ivec3& chunkBlockCoords
        );

      private:
        std::array<LayerBuffer, 3> m_renderLayers;

        std::atomic_bool m_generated{false};
        std::atomic_bool m_generatingMesh{false};
        std::atomic_bool m_meshReady{false};
        std::mutex m_backMutex;

        friend class World;
    };


}  // namespace engine
