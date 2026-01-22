#pragma once

#include <glm/glm.hpp>
#include <iostream>

#include "ChunkData.h"
#include "LayerBuffer.h"
#include "block/Block.h"
#include "block/BlockState.h"
#include "block/VariantBlock.h"
#include "block/Vertex.h"
#include "render/Renderable.h"

#include <LWGL/buffer/Attributes.h>


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


    struct ChunkID : glm::ivec3 {
        ChunkID(int x, int y, int z) : glm::ivec3(x, y, z) {}
        ChunkID(const glm::ivec3& vec) : glm::ivec3(vec) {}

        bool operator==(const ChunkID& other) const;
        std::string toString() const;
    };


    class Chunk : public Renderable {
      public:
        static inline glm::ivec3 Dims{16, 16, 16};

        Chunk(World* world, ChunkID coords, glm::ivec3 dims = Dims);
        Chunk(Chunk&) = delete;
        Chunk(Chunk&&) = delete;
        ~Chunk();

        /// @return ID or coordinates of the chunk in the world.
        const ChunkID& id() const { return m_coords; }
        /// @return Position of the chunk in world space.
        glm::ivec3 position() const { return m_coords * m_data.dims; }

        /// @brief Get the dimensions of this chunk type.
        /// @return The dimensions (x, y, z) of the chunk.
        /// @note Override this in derived classes to provide custom chunk dimensions.
        virtual glm::ivec3 dims() const { return m_data.dims; }

        World* world() const { return m_world; }

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
        ChunkData& data() { return m_data; }
        /// @return const ChunkData structure containing all the block/terrain data.
        const ChunkData& data() const { return m_data; }


        /// @brief Renders the chunk.
        /// @param pass Pass == 0 will render the whole chunk, pass == 1 will render opaque blocks, pass == 2 will render transparent blocks.
        void render(Engine& engine, const Camera* camera, int pass) override;

        VariantBlock::Neighbours getNeighbouringBlocks(glm::ivec3 pos) const;

        /// @section Events
        // TODO probably remove this
        virtual void afterGenerated() {}


      protected:
        World* m_world;
        ChunkID m_coords;
        ChunkData m_data;
        bool m_dirty = false;

        struct GeometryState {
            glm::vec3 axis;
            float angle;
        };

        struct MeshGenContext {
            const Block* block;
            const Geometry* geometry;
            GeometryState geometryState;
            gl::Attributes<Vertex>& storage;
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
        std::array<LayerBuffer, 4> m_renderLayers;
        // gl::Attributes<Vertex> m_opaqueVertData;
        // gl::Attributes<Vertex> m_transparentVertData;
        // gl::Attributes<Vertex> m_backOpaqueVertData;
        // gl::Attributes<Vertex> m_backTransparentVertData;

        bool m_generated = false;
        std::atomic_bool m_generatingMesh = false;

        friend class World;
    };


}  // namespace engine


std::ostream& operator<<(std::ostream& os, const engine::ChunkID& chID);

namespace std {
    template <>
    struct hash<engine::ChunkID> {
        size_t operator()(const engine::ChunkID& k) const {
            return hash<int>()(k.x) ^ hash<int>()(k.y << 1) ^ hash<int>()(k.z << 2);
        }
    };

}  // namespace std
