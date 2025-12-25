#pragma once

#include "../Biome.h"

#include <data/RegistryManager.h>

class FrozenGasFieldsBiome : public Biome {
  public:
    FrozenGasFieldsBiome()
        : Biome("FrozenGasFields", {-1.0f, -0.25f}, {0.2f, 1.0f}, ParameterRange::High, 60) {
        auto& registry = RegistryManager::Blocks();

        m_descriptor.heightScale = 6.0f;
        m_descriptor.heightBase = 90.0f;

        m_descriptor.layers.push_back(
            {.palette = {{registry.get("gas_ice")->getID()}, {1}}, .depth = 4, .features = {}}
        );

        m_descriptor.layers.push_back(
            {.palette = {{registry.get("slate")->getID()}, {1}}, .depth = -1, .features = {}}
        );
    }

    // float getHeightModifier(int x, int z, float baseHeight) const {
    //     // Add dune pattern
    //     float duneHeight = m_duneNoise.get(x, z) * 3.0f;
    //     return (baseHeight * m_descriptor.heightScale + duneHeight) + m_descriptor.heightBase;
    // }

    // BlockID getBlockAt(const glm::ivec3& pos, int surfaceHeight) const override {}
};