#include "FlatTerrainGenerator.h"

#include "Chunk.h"
#include "data/RegistryManager.h"

using namespace engine;

FlatTerrainGenerator::FlatTerrainGenerator() : m_blockRegistry(RegistryManager::Blocks()) {}


BlockID FlatTerrainGenerator::voxelAt(const glm::ivec3& pos) {
    if (pos.y > 0) {
        return m_blockRegistry.get("air")->getID();
    }

    return 1;
}

void FlatTerrainGenerator::populate(Chunk& chunk) {
    auto& data = chunk.data();
    glm::ivec3 chunkDims = chunk.dims();
    auto chunkCoords = chunk.id() * chunkDims;
    for (int x = 0; x < chunkDims.x; ++x) {
        for (int y = 0; y < chunkDims.y; ++y) {
            for (int z = 0; z < chunkDims.z; ++z) {
                BlockID blockID = voxelAt(glm::ivec3(x, y, z) + chunkCoords);
                if (blockID != 0) {
                    data.setBlock({x, y, z}, blockID);
                }
            }
        }
    }
}