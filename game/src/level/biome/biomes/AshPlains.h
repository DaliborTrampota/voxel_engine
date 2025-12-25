#pragma once

#include "../Biome.h"

#include <data/RegistryManager.h>

class AshPlainsBiome : public Biome {
  public:
    AshPlainsBiome() : Biome("AshPlains", {-1.0f, 1.0f}, {-1.0f, 1.0f}, ParameterRange::Low) {
        auto& registry = RegistryManager::Blocks();

        m_descriptor.heightScale = 10.0f;
        m_descriptor.heightBase = 62.0f;

        m_descriptor.layers.push_back(
            {.palette = {{registry.get("ash")->getID()}, {1}}, .depth = 8, .features = {}}
        );
        m_descriptor.layers.push_back(
            {.palette = {{registry.get("compressed_ash")->getID()}, {1}},
             .depth = 30,
             .features = {}}
        );
        m_descriptor.layers.push_back(
            {.palette = {{registry.get("slate")->getID()}, {1}}, .depth = -1, .features = {}}
        );
    }

    // BlockID getBlockAt(const glm::ivec3& pos, int surfaceHeight) const override {}
};