#pragma once

#include "../Biome.h"

#include <data/RegistryManager.h>

class SilicateSandSeasBiome : public Biome {
  public:
    SilicateSandSeasBiome()
        : Biome("SilicateSandSeas", {0.25f, 1.0f}, {0.1f, 0.3f}, ParameterRange::Low, 10) {
        auto& registry = RegistryManager::Blocks();

        m_descriptor.heightScale = 20.0f;
        m_descriptor.heightBase = 64.0f;

        m_descriptor.layers.push_back(
            {.palette = {{registry.get("silicate_sand")->getID()}, {1}}, .depth = -1, .features = {}}
        );
    }

    // float getHeightModifier(int x, int z, float baseHeight) const {
    //     // Add dune pattern
    //     float duneHeight = m_duneNoise.get(x, z) * 3.0f;
    //     return (baseHeight * m_descriptor.heightScale + duneHeight) + m_descriptor.heightBase;
    // }

    // BlockID getBlockAt(const glm::ivec3& pos, int surfaceHeight) const override {}
};