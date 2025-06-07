#pragma once

#include <glm/glm.hpp>

#include <thread>
#include <unordered_map>
#include <vector>

#include "block/Block.h"
#include "block/BlockData.h"
#include "render/Renderable.h"

#include <core/gl/geometry/Attributes.h>


namespace gl {
    class ShaderPipeline;
}

namespace engine {

    class World;
    class Engine;
    struct RenderContext;

    using T = unsigned int;
    using VoxelData = std::vector<std::vector<std::vector<T>>>;


    struct ChunkID : glm::ivec3 {
        ChunkID(int x, int y, int z) : glm::ivec3(x, y, z) {}
        ChunkID(const glm::ivec3& vec) : glm::ivec3(vec) {}

        bool operator()(const ChunkID& a, const ChunkID& b) const;
        bool operator==(const ChunkID& other) const;
    };


    class Chunk : public Renderable {
      public:
        static inline glm::ivec3 Dims{16, 16, 16};

        Chunk(World* world, ChunkID coords);
        Chunk(Chunk&) = delete;
        Chunk(Chunk&&) = delete;
        ~Chunk();

        /// @return ID or coordinates of the chunk in the world.
        const ChunkID& id() const { return m_coords; }

        /// @brief Generates the chunk data per TerrainGenerator if not generated yet.
        void generate();
        bool generated() const { return m_generated; }

        /// @brief Generates the mesh data for the chunk.
        /// @return true if the mesh was generated, false if it was already generating.
        /// @note This function is thread-safe.
        bool generateMesh();


        Block getBlock(glm::ivec3 pos) const;

        /// @return 3D vector of the block data.
        VoxelData& data() { return m_data; }


        /// @brief Renders the chunk.
        /// @param pass Pass == 0 will render the whole chunk, pass == 1 will render opaque blocks, pass == 2 will render transparent blocks.
        void render(Engine& engine, const Camera* camera, int pass) override;

      private:
        World* m_world;
        ChunkID m_coords;

        VoxelData m_data;
        gl::Attributes<Vertex> m_vertexData;

        bool m_generated = false;
        std::atomic_bool m_generatingMesh = false;

        friend class Engine;
        friend class World;
    };


}  // namespace engine

namespace std {
    template <>
    struct hash<engine::ChunkID> {
        size_t operator()(const engine::ChunkID& k) const {
            return hash<int>()(k.x) ^ hash<int>()(k.y << 1) ^ hash<int>()(k.z << 2);
        }
    };

}  // namespace std
