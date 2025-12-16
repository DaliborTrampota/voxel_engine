#include "PerlinTerrainGenerator.h"

#include <Globals.h>
#include <data/RegistryManager.h>
#include <level/Chunk.h>
#include <glm/gtc/random.hpp>

#include <utility/Random.h>


using namespace engine;

PerlinTerrainGenerator::PerlinTerrainGenerator(
    siv::PerlinNoise::seed_type seed, float scale, int octaves, float persistence
)
    : m_noise(seed),
      m_blockRegistry(RegistryManager::Blocks()),
      m_scale(scale),
      m_octaves(octaves),
      m_persistence(persistence) {}


int PerlinTerrainGenerator::height(int x, int z) const {
    return static_cast<int>(
               m_noise.normalizedOctave2D_01(x * m_scale, z * m_scale, m_octaves, m_persistence) *
               8.0f
           ) +
           m_minHeight;
}

BlockID PerlinTerrainGenerator::voxelAt(const glm::ivec3& pos) {
    int h = height(pos.x, pos.z);
    return voxelAt(pos, h);
}

BlockID PerlinTerrainGenerator::voxelAt(const glm::ivec3& pos, int height) {
    int dirtHeight = static_cast<int>(m_noise.noise2D_01(pos.x * m_scale, pos.y * m_scale) * 2) + 3;
    // if (pos.y == height + 1 && glm::linearRand(.0f, 1.0f) < 0.4f)
    //     return m_blockRegistry.get("pyramid").getID();

    if (pos.y == height + 1) {
        if (Random::random2D(pos.x, pos.z) < 0.3f)
            return m_blockRegistry.get("grass_deco")->getID();
    }
    if (pos.y > height)
        return 0;

    unsigned int blockID;
    if (pos.y == height || pos.y == height - 1)
        blockID = m_blockRegistry.get("grass")->getID();
    else if (pos.y > dirtHeight)
        blockID = m_blockRegistry.get("dirt")->getID();
    else
        blockID = m_blockRegistry.get("stone")->getID();

    return blockID;
}


void PerlinTerrainGenerator::populate(Chunk& chunk) {
    auto& data = chunk.data();
    glm::ivec3 chunkCoords = chunk.id() * Chunk::Dims;
    for (int x = 0; x < Chunk::Dims.x; x++) {
        for (int z = 0; z < Chunk::Dims.z; z++) {
            int h = height(x + chunkCoords.x, z + chunkCoords.z);
            for (int y = 0; y < Chunk::Dims.y; y++) {
                BlockID blockID = voxelAt(glm::ivec3(x, y, z) + chunkCoords, h);
                if (blockID != 0) {
                    //const Block* block = m_blockRegistry.get(blockID);

                    data.setBlock(x, y, z, blockID);
                }
            }
        }
    }
}
