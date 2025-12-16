#pragma once

#include "../Biome.h"

#include <data/RegistryManager.h>

class MountainBiome : public Biome {
  public:
    MountainBiome() : Biome("Mountains", {-1.0f, 0.0f}, {0.0f, 1.0f}, {0.5f, 1.0f}) {
        auto& registry = RegistryManager::Blocks();

        m_descriptor.heightScale = 32.0f;  // High amplitude
        m_descriptor.heightBase = 80.0f;   // Higher base

        m_descriptor.layers.push_back(
            {.palette = {{registry.get("stone")->getID()}, {1}}, .depth = 100, .features = {}}
        );
    }
    // float getHeightModifier(int x, int z, float baseHeight) const override {
    //     // Amplify height variation for mountains
    //     // Use power function to make peaks sharper
    //     float h = baseHeight;
    //     h = glm::pow(h, 1.5f);  // Make peaks more dramatic
    //     return h * m_descriptor.heightScale + m_descriptor.heightBase;
    // }


    // BlockID getBlockAt(const glm::ivec3& pos, int surfaceHeight) const override {}
};