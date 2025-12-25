#pragma once

#include "../Biome.h"

#include <data/RegistryManager.h>

class SlateRockFieldsBiome : public Biome {
  public:
    SlateRockFieldsBiome()
        : Biome("SlateRockFields", {0.05f, 1.0f}, {-0.5f, 0.2f}, ParameterRange::High, 10) {
        auto& registry = RegistryManager::Blocks();

        m_descriptor.heightScale = 20.0f;
        m_descriptor.heightBase = 90.0f;

        m_descriptor.layers.push_back(
            {.palette = {{registry.get("slate")->getID()}, {1}}, .depth = 10, .features = {}}
        );

        m_descriptor.layers.push_back(
            {.palette = {{registry.get("deep_slate")->getID()}, {1}}, .depth = -1, .features = {}}
        );
    }

    // float getHeightModifier(int x, int z, float baseHeight) const {
    //     // Add dune pattern
    //     float duneHeight = m_duneNoise.get(x, z) * 3.0f;
    //     return (baseHeight * m_descriptor.heightScale + duneHeight) + m_descriptor.heightBase;
    // }

    // BlockID getBlockAt(const glm::ivec3& pos, int surfaceHeight) const override {}
};