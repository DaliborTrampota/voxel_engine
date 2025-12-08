#pragma once

#include <glm/glm.hpp>

#include <level/ITerrainGenerator.h>

#include <block/Block.h>
#include <data/Registry.h>
#include <tools/PerlinNoise.h>

using namespace engine;

class PerlinTerrainGenerator : public ITerrainGenerator {
  public:
    PerlinTerrainGenerator(siv::PerlinNoise::seed_type seed);

    BlockID voxelAt(const glm::ivec3& pos) override;
    void populate(Chunk& chunk) override;

    BlockID voxelAt(const glm::ivec3& pos, int height);
    int height(int x, int z) const;

  protected:
    siv::BasicPerlinNoise<float> m_noise;
    const Registry<Block, RegistryStoragePolicy::ByPointer>& m_blockRegistry;

    float m_scale = 0.1f;
    float m_minHeight = 5.0f;
};
