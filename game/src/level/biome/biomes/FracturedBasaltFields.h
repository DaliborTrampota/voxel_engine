#pragma once

#include "../Biome.h"

#include <data/RegistryManager.h>

class FracturedBasaltFieldsBiome : public Biome {
  public:
    FracturedBasaltFieldsBiome()
        : Biome("FracturedBasaltFields", {0.0f, 1.0f}, {-1.0f, 0.1f}, ParameterRange::Low, 10) {
        auto& registry = RegistryManager::Blocks();

        m_descriptor.heightScale = 3.0f;
        m_descriptor.heightBase = 50.0f;

        m_descriptor.layers.push_back(
            {.palette = {{registry.get("basalt")->getID()}, {1}}, .depth = -1, .features = {}}
        );
    }

    // BlockID getBlockAt(const glm::ivec3& pos, int surfaceHeight) const override {}
};