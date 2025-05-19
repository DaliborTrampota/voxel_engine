#include "Chunk.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <core/gl/Attributes.h>
#include <core/gl/ShaderPipeline.h>

#include "block/Vertex.h"
#include "data/VertexData.h"

#include "World.h"
#include "block/Block.h"
#include "data/RegistryManager.h"
//#include "block/builder/CulledGeometry.h"
//#include "block/builder/CubeGeometry.h"


using namespace engine;

Chunk::Chunk(World* world, ChunkID coords)
    : m_world(world),
      m_coords(coords),
      m_vertexData(GL_DYNAMIC_DRAW) {
    m_vertexData.create();
}

Chunk::~Chunk() {}

void Chunk::generate() {
    if (m_generated)
        return;

    m_data = VoxelData(Dims.x, std::vector<std::vector<T>>(Dims.y, std::vector<T>(Dims.z, 0)));
    m_world->m_generator->populate(*this);
}

bool Chunk::generateMesh() {
    bool expected = false;
    if (!m_generatingMesh.compare_exchange_strong(expected, true))
        return false;

    glm::ivec3 chunkBlockCoords = m_coords * Chunk::Dims;
    for (int x = 0; x < Chunk::Dims.x; x++) {
        m_vertexData.reserve(
            Chunk::Dims.x * Chunk::Dims.y * Chunk::Dims.z * 6
        );  // 16x16x6 faces (6 vertices per face)

        for (int y = 0; y < Chunk::Dims.y; y++) {
            for (int z = 0; z < Chunk::Dims.z; z++) {
                if (m_data[x][y][z] == 0)
                    continue;

                glm::ivec3 pos(x, y, z);
                int blockID = m_data[x][y][z];

                Block block = RegistryManager::Blocks().get(blockID);

                for (auto f : block.geometry()->faces()) {
                    if (f.cull && m_world->checkBlock(
                                        pos + f.cullDir + chunkBlockCoords, block, f.cullDir
                                    ))
                        continue;

                    f.translate(pos);

                    for (Vertex v : f.vertices) {
                        v.data(block.material().forTag(f.tag), 0);
                        m_vertexData.add(v);
                    }
                }
            }
        }
    }
    m_generatingMesh = false;
    m_vertexData.m_data.shrink_to_fit();
    m_generated = true;
    return true;
}

Block Chunk::getBlock(glm::ivec3 pos) const {
    T blockID = m_data.size() == 0 ? m_world->m_generator->voxelAt(pos + m_coords * Chunk::Dims)
                                   : m_data[pos.x][pos.y][pos.z];
    return RegistryManager::Blocks().get(blockID);
}

bool ChunkID::operator()(const ChunkID& a, const ChunkID& b) const {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

bool ChunkID::operator==(const ChunkID& other) const {
    return x == other.x && y == other.y && z == other.z;
}
