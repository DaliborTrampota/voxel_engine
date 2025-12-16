#pragma once

#include "../Biome.h"

#include <data/RegistryManager.h>

class SwampBiome : public Biome {
  public:
    SwampBiome() : Biome("Swamp", {0.0f, 1.0f}, {0.5f, 1.0f}, {-1.0f, -0.3f}) {
        auto& registry = RegistryManager::Blocks();

        m_descriptor.heightScale = 2.0f;  // Very flat
        m_descriptor.heightBase = 62.0f;  // Below sea level

        m_descriptor.layers.push_back(
            {.palette = {{registry.get("dirt")->getID()}, {1}}, .depth = 1, .features = {}}
        );
        m_descriptor.layers.push_back(
            {.palette = {{registry.get("dirt")->getID()}, {1}}, .depth = 3, .features = {}}
        );
        m_descriptor.layers.push_back(
            {.palette = {{registry.get("stone")->getID()}, {1}}, .depth = 100, .features = {}}
        );
    }

    // BlockID getBlockAt(const glm::ivec3& pos, int surfaceHeight) const override {}
};