#pragma once

#include "../Biome.h"

#include <data/RegistryManager.h>

class BasaltPlateausBiome : public Biome {
  public:
    BasaltPlateausBiome()
        : Biome("BasaltPlateaus", {0.0f, 1.0f}, {-1.0f, 0.1f}, ParameterRange::Mid, 0) {
        auto& registry = RegistryManager::Blocks();

        m_descriptor.heightScale = 5.0f;
        m_descriptor.heightBase = 62.0f;

        m_descriptor.layers.push_back(
            {.palette = {{registry.get("iron_basalt")->getID()}, {1}}, .depth = -1, .features = {}}
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