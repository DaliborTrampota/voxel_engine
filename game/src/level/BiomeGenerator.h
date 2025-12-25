#pragma once

#include <glm/glm.hpp>

#include <level/ITerrainGenerator.h>

#include <block/Block.h>
#include <data/Registry.h>
#include <utility/PerlinNoise.h>

#include <level/Chunk.h>

#include "biome/BiomeDescriptor.h"

using namespace engine;

class Biome;

struct BiomeBlendData {
    Biome* biome;
    float weight;
};

class BiomeGenerator : public ITerrainGenerator {
  public:
    BiomeGenerator(siv::PerlinNoise::seed_type seed);

    int height(int x, int z);
    BlockID voxelAt(const glm::ivec3& pos) override;
    Biome* biomeAt(int x, int z);
    void populate(Chunk& chunk) override;


    void add(std::unique_ptr<Biome>&& biome);

    void setBlendRadius(float radius) { m_blendRadius = radius; }
    void setEnableBlending(bool enable) { m_enableBlending = enable; }
    std::vector<BiomeBlendData> getBiomeBlend(int x, int z, float blendRadius = 32.0f);

  protected:
    PerlinNoise m_height;
    PerlinNoise m_temperature;
    PerlinNoise m_concentration;
    PerlinNoise m_altitude;

    Parameter m_lowAltitude = {-1.0f, -0.15f};
    Parameter m_highAltitude = {0.35f, 1.0f};

    Registry<Block, RegistryStoragePolicy::ByPointer>& m_blockRegistry;

    std::vector<std::unique_ptr<Biome>> m_biomes;

    std::vector<int> m_biomeMap;

    float m_blendRadius = 32.0f;
    bool m_enableBlending = true;
};
