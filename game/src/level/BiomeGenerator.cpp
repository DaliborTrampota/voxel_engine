#include "BiomeGenerator.h"

#include <Globals.h>
#include <data/RegistryManager.h>
#include <level/Chunk.h>
#include <utility/Random.h>

#include "biome/Biome.h"

#include <algorithm>
#include <glm/gtc/random.hpp>
#include <memory>


using namespace engine;

constexpr int worldScale = 10;

BiomeGenerator::BiomeGenerator(siv::PerlinNoise::seed_type seed)
    : m_height(seed, 50, 0.0f, 5, 0.4f),
      m_temperature(seed + 1, 15 * worldScale, 0.0f, 3, 0.3f),
      m_concentration(seed + 2, 30 * worldScale, 0.0f, 3, 0.3f),
      m_altitude(seed + 3, 100 * worldScale, 0.0f, 1, 0.3f),
      m_blockRegistry(RegistryManager::Blocks()) {}


// Get final height with biome blending
int BiomeGenerator::height(int x, int z) const {
    // return 64;
    float baseHeight = m_height.get(x, z);

    if (!m_enableBlending) {
        // No blending - use dominant biome
        Biome* biome = biomeAt(x, z);
        if (biome) {
            return static_cast<int>(biome->getHeightModifier(x, z, baseHeight));
        }
        return static_cast<int>(baseHeight * 8.0f) + 5;
    }

    // Blend heights from nearby biomes
    auto blendData = getBiomeBlend(x, z, m_blendRadius);

    if (blendData.empty()) {
        return static_cast<int>(baseHeight * 8.0f) + 5;
    }

    float blendedHeight = 0.0f;
    for (const auto& data : blendData) {
        float biomeHeight = data.biome->getHeightModifier(x, z, baseHeight);
        blendedHeight += biomeHeight * data.weight;
    }

    return static_cast<int>(blendedHeight);
}

BlockID BiomeGenerator::voxelAt(const glm::ivec3& pos) {
    int h = height(pos.x, pos.z);

    if (pos.x == 9 && pos.z == 6) {
        printf("Voxel at: %d, %d, %d, %d\n", pos.x, pos.y, pos.z, h);
    }

    // Get dominant biome for block placement
    Biome* biome = biomeAt(pos.x, pos.z);

    if (!biome) {
        return 0;  // No biome, return air
    }

    // Use the biome's block placement logic
    // if (pos.y == h) {
    //     return biome->descriptor().layers[0].palette.blocks[0];
    // }
    return biome->getBlockAt(pos, h);
}


void BiomeGenerator::populate(Chunk& chunk) {
    //m_chunkID = chunk.id();
    auto& data = chunk.data();
    glm::ivec3 chunkCoords = chunk.id() * Chunk::Dims;

    for (int x = 0; x < Chunk::Dims.x; x++) {
        for (int z = 0; z < Chunk::Dims.z; z++) {
            glm::ivec3 worldPos = glm::ivec3(x, 0, z) + chunkCoords;
            int h = height(worldPos.x, worldPos.z);

            for (int y = 0; y < Chunk::Dims.y; y++) {
                worldPos.y = y + chunkCoords.y;
                BlockID blockID = voxelAt(worldPos);

                if (blockID != 0) {
                    data.setBlock(x, y, z, blockID);
                }
            }
        }
    }
}


Biome* BiomeGenerator::biomeAt(int x, int z) const {
    if (m_biomes.empty()) {
        return nullptr;
    }

    float t = m_temperature.getClamped(x, z);
    float c = m_concentration.getClamped(x, z);
    float a = m_altitude.getClamped(x, z);

    struct {
        Biome* biome;
        float score = std::numeric_limits<float>::max();
        int priority = 0;
    } bestBiome;

    for (const auto& biome : m_biomes) {
        float score = biome->fitScore(t, c, a);

        if (score < bestBiome.score ||
            (score == bestBiome.score && biome->priority() > bestBiome.priority)) {
            bestBiome.biome = biome.get();
            bestBiome.score = score;
            bestBiome.priority = biome->priority();
        }
    }

    if (x == 9 && z == 6) {
        printf("Best biome: %s\n", bestBiome.biome->name().c_str());
    }

    return bestBiome.biome;
}

// Get nearby biomes with blend weights for smooth transitions
std::vector<BiomeBlendData> BiomeGenerator::getBiomeBlend(int x, int z, float blendRadius) const {
    if (m_biomes.empty()) {
        return {};
    }

    float t = m_temperature.getClamped(x, z);
    float c = m_concentration.getClamped(x, z);
    float a = m_altitude.getClamped(x, z);

    // Calculate fit scores for all biomes
    std::vector<BiomeBlendData> blendData;
    float totalInverseDistance = 0.0f;

    for (const auto& biome : m_biomes) {
        float score = biome->fitScore(t, c, a);

        // Convert distance to weight (closer = higher weight)
        // Use inverse square for smoother falloff
        if (score < blendRadius) {
            float weight = 1.0f / (1.0f + score * score);
            blendData.push_back({biome.get(), weight});
            totalInverseDistance += weight;
        }
    }

    // Normalize weights so they sum to 1.0
    if (totalInverseDistance > 0.0f) {
        for (auto& data : blendData) {
            data.weight /= totalInverseDistance;
        }
    }

    // Sort by weight (highest first) and keep only top contributors
    std::sort(
        blendData.begin(), blendData.end(), [](const BiomeBlendData& a, const BiomeBlendData& b) {
            return a.weight > b.weight;
        }
    );

    // Keep only significant contributors (top 3 biomes with weight > 5%)
    auto it = std::remove_if(blendData.begin(), blendData.end(), [](const BiomeBlendData& data) {
        return data.weight < 0.05f;
    });
    blendData.erase(it, blendData.end());

    // Limit to 3 biomes maximum for performance
    if (blendData.size() > 3) {
        blendData.resize(3);

        // Re-normalize after limiting
        float totalWeight = 0.0f;
        for (const auto& data : blendData) {
            totalWeight += data.weight;
        }
        for (auto& data : blendData) {
            data.weight /= totalWeight;
        }
    }

    return blendData;
}

void BiomeGenerator::add(std::unique_ptr<Biome> biome) {
    m_biomes.push_back(std::move(biome));
}