#pragma once

#include "../Biome.h"

#include <data/RegistryManager.h>

class AlgalPlainsBiome : public Biome {
  public:
    AlgalPlainsBiome()
        : Biome("AlgalPlains", {-0.1f, 0.2f}, {-1.0f, -0.2f}, ParameterRange::Mid, 10) {
        auto& registry = RegistryManager::Blocks();

        m_descriptor.heightScale = 15.0f;
        m_descriptor.heightBase = 64.0f;

        m_descriptor.layers.push_back(
            {.palette = {{registry.get("algal_slime")->getID()}, {1}}, .depth = 3, .features = {}}
        );
        m_descriptor.layers.push_back(
            {.palette = {{registry.get("ash")->getID()}, {1}}, .depth = 6, .features = {}}
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