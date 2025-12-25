#include "BiomeGenerator.h"

#include <Globals.h>
#include <data/RegistryManager.h>
#include <level/Chunk.h>
#include <utility/Random.h>

#include "biome/Biome.h"

#include <algorithm>
#include <glm/gtc/random.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>
#include <memory>


using namespace engine;

constexpr int worldScale = 5;

BiomeGenerator::BiomeGenerator(siv::PerlinNoise::seed_type seed)
    : m_height(seed, 50, 0.0f, 5, 0.4f),
      m_temperature(seed + 1, 30 * worldScale, 0.0f, 3, 0.3f),
      m_concentration(seed + 2, 30 * worldScale, 0.0f, 3, 0.3f),
      m_altitude(seed + 3, 80 * worldScale, 0.0f, 1, 0.3f),
      m_blockRegistry(RegistryManager::Blocks()) {
    m_biomeMap.resize(Chunk::Dims.x * Chunk::Dims.z);
}


// Get final height with biome blending
int BiomeGenerator::height(int x, int z) {
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

    if (blendData.size() == 1) {
        return static_cast<int>(blendData[0].biome->getHeightModifier(x, z, baseHeight));
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

    Biome* biome = biomeAt(pos.x, pos.z);
    if (!biome) {
        return Block::AirID;
    }
    return biome->getBlockAt(pos, h);
}


void BiomeGenerator::populate(Chunk& chunk) {
    auto& data = chunk.data();
    glm::ivec3 chunkCoords = chunk.id() * Chunk::Dims;

    for (int x = 0; x < Chunk::Dims.x; x++) {
        for (int z = 0; z < Chunk::Dims.z; z++) {
            glm::ivec3 worldPos = glm::ivec3(x, 0, z) + chunkCoords;
            int h = height(worldPos.x, worldPos.z);
            Biome* biome = biomeAt(worldPos.x, worldPos.z);
            if (!biome) {
                continue;
            }
            for (int y = 0; y < Chunk::Dims.y; y++) {
                worldPos.y = y + chunkCoords.y;
                BlockID blockID = biome->getBlockAt(worldPos, h);
                if (blockID != Block::AirID) {
                    data.setBlock(x, y, z, blockID);
                }
            }
        }
    }
}


Biome* BiomeGenerator::biomeAt(int x, int z) {
    if (m_biomes.empty()) {
        return nullptr;
    }
    if (x >= 0 && x < Chunk::Dims.x && z >= 0 && z < Chunk::Dims.z) {
        int biomeIdx = m_biomeMap.at(x * Chunk::Dims.z + z);
        if (biomeIdx != -1) {
            return m_biomes.at(biomeIdx).get();
        }
    }

    float t = m_temperature.getClamped(x, z);
    float c = m_concentration.getClamped(x, z);
    float a = m_altitude.getClamped(x, z);

    Biome* bestBiome = nullptr;
    float bestScore = std::numeric_limits<float>::max();
    int bestPriority = 0;
    int bestIdx = -1;


    for (int i = 0; i < m_biomes.size(); i++) {
        Biome* biome = m_biomes.at(i).get();
        switch (biome->altitude()) {
            case ParameterRange::Low:
                if (a > m_lowAltitude.max)
                    continue;
                break;
            case ParameterRange::Mid:
                if (a < m_lowAltitude.max || a > m_highAltitude.min)
                    continue;
                break;
            case ParameterRange::High:
                if (a < m_highAltitude.min)
                    continue;
                break;
        }
        float score = biome->fitScore(t, c);

        if (score < bestScore || (score == bestScore && biome->priority() > bestPriority)) {
            bestBiome = biome;
            bestScore = score;
            bestPriority = biome->priority();
            bestIdx = i;
        }
    }
    if (x >= 0 && x < Chunk::Dims.x && z >= 0 && z < Chunk::Dims.z) {
        m_biomeMap.at(x * Chunk::Dims.z + z) = bestIdx;
    }
    return bestBiome;
}

std::vector<BiomeBlendData> BiomeGenerator::getBiomeBlend(int x, int z, float blendRadius) {
    if (m_biomes.empty()) {
        return {};
    }

    Biome* currentBiome = biomeAt(x, z);
    std::unordered_map<Biome*, int> biomeWeights;
    int totalSamples = 0;
    int diffSamples = 0;

    const int blendRadiusSq = blendRadius * blendRadius;
    for (int i = -blendRadius; i <= blendRadius; i++) {
        for (int j = -blendRadius; j <= blendRadius; j++) {
            if (glm::length2(glm::vec2(i, j)) > blendRadiusSq) {
                continue;
            }


            totalSamples += 1;

            Biome* biome = biomeAt(x + i, z + j);
            if (biome == currentBiome)
                continue;

            if (!biomeWeights.contains(biome))
                biomeWeights[biome] = 0;
            biomeWeights[biome] += 1;
            // Calculate weight based on distance (closer to edge = higher weight)
            // Use inverse distance squared for smooth falloff
            // float weight = 1.0f / (1.0f + distance * distance);

            diffSamples += 1;
        }
    }
    if (diffSamples == 0)
        return {{currentBiome, 1.0f}};

    std::vector<BiomeBlendData> blendData;
    float totalWeight = 0.0f;

    float centerWeight = (totalSamples - diffSamples) / static_cast<float>(totalSamples);
    totalWeight += centerWeight;
    blendData.push_back({currentBiome, centerWeight});

    for (const auto& [biome, weight] : biomeWeights) {
        float blendWeight = static_cast<float>(weight) / static_cast<float>(totalSamples);
        totalWeight += blendWeight;
        blendData.push_back({biome, blendWeight});
    }
    float t = glm::abs(totalWeight - 1.0f);
    float eps = std::numeric_limits<float>::epsilon();
    assert(t <= eps);


    // // Sort by weight (highest first) and keep only top contributors
    // std::sort(
    //     blendData.begin(), blendData.end(), [](const BiomeBlendData& a, const BiomeBlendData& b) {
    //         return a.weight > b.weight;
    //     }
    // );

    // // Keep only significant contributors (top 3 biomes with weight > 5%)
    // auto it = std::remove_if(blendData.begin(), blendData.end(), [](const BiomeBlendData& data) {
    //     return data.weight < 0.05f;
    // });
    // blendData.erase(it, blendData.end());

    // // Limit to 3 biomes maximum for performance
    // if (blendData.size() > 3) {
    //     blendData.resize(3);

    //     // Re-normalize after limiting
    //     float totalWeight = 0.0f;
    //     for (const auto& data : blendData) {
    //         totalWeight += data.weight;
    //     }
    //     for (auto& data : blendData) {
    //         data.weight /= totalWeight;
    //     }
    // }

    return blendData;
}

void BiomeGenerator::add(std::unique_ptr<Biome>&& biome) {
    m_biomes.push_back(std::move(biome));
}
