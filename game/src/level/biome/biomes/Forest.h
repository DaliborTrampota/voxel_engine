#pragma once

#include "../Biome.h"

#include <data/RegistryManager.h>

class ForestBiome : public Biome {
  public:
    ForestBiome() : Biome("Forest", {-0.3f, 0.6f}, {0.2f, 1.0f}, {-0.5f, 0.5f}) {
        auto& registry = RegistryManager::Blocks();

        m_descriptor.heightScale = 8.0f;
        m_descriptor.heightBase = 64.0f;


        // m_descriptor.layers.push_back(
        //     {.palette = {{0, registry.get("grass_deco")->getID()}, {50, 1}},
        //      .depth = -1,
        //      .features = {}}
        // );
        m_descriptor.layers.push_back(
            {.palette = {{registry.get("leaves")->getID()}, {1}}, .depth = 1, .features = {}}
        );
        m_descriptor.layers.push_back(
            {.palette = {{registry.get("dirt")->getID()}, {1}}, .depth = 4, .features = {}}
        );

        m_descriptor.layers.push_back(
            {.palette = {{registry.get("stone")->getID()}, {1}}, .depth = 100, .features = {}}
        );
    }

    // BlockID getBlockAt(const glm::ivec3& pos, int surfaceHeight) const override {}
};