#pragma once

#include <glm/glm.hpp>

#include <level/ITerrainGenerator.h>

#include <block/Block.h>
#include <data/Registry.h>
#include <tools/PerlinNoise.h>

using namespace engine;

class PerlinTerrainGenerator : public ITerrainGenerator {
  public:
    PerlinTerrainGenerator(
        siv::PerlinNoise::seed_type seed, float scale, int octaves, float persistence = 0.5f
    );

    BlockID voxelAt(const glm::ivec3& pos) override;
    void populate(Chunk& chunk) override;

    BlockID voxelAt(const glm::ivec3& pos, int height);
    int height(int x, int z) const;

  protected:
    siv::BasicPerlinNoise<float> m_noise;
    const Registry<Block, RegistryStoragePolicy::ByPointer>& m_blockRegistry;

    float m_scale;
    int m_octaves;
    float m_persistence;
    float m_minHeight = 5.0f;
};
