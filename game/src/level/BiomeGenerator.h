#pragma once

#include <glm/glm.hpp>

#include <level/ITerrainGenerator.h>

#include <block/Block.h>
#include <data/Registry.h>
#include <utility/PerlinNoise.h>

#include <level/Chunk.h>

using namespace engine;

class Biome;

struct BiomeBlendData {
    Biome* biome;
    float weight;
};

class BiomeGenerator : public ITerrainGenerator {
  public:
    BiomeGenerator(siv::PerlinNoise::seed_type seed);

    BlockID voxelAt(const glm::ivec3& pos) override;
    void populate(Chunk& chunk) override;

    // Get height at position (blended from nearby biomes)
    int height(int x, int z) const;

    // Get the dominant biome at a position
    Biome* biomeAt(int x, int z) const;

    // Get nearby biomes with blend weights (for smooth transitions)
    std::vector<BiomeBlendData> getBiomeBlend(int x, int z, float blendRadius = 32.0f) const;

    void add(std::unique_ptr<Biome> biome);

    // Control blending behavior
    void setBlendRadius(float radius) { m_blendRadius = radius; }
    void setEnableBlending(bool enable) { m_enableBlending = enable; }

  protected:
    PerlinNoise m_height;
    PerlinNoise m_temperature;
    PerlinNoise m_concentration;
    PerlinNoise m_altitude;

    //ChunkID m_chunkID;

    const Registry<Block, RegistryStoragePolicy::ByPointer>& m_blockRegistry;

    std::vector<std::unique_ptr<Biome>> m_biomes;

    float m_blendRadius = 32.0f;
    bool m_enableBlending = true;
};
