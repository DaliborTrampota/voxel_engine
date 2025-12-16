#pragma once

#include "../Biome.h"

#include <data/RegistryManager.h>

class PlainsBiome : public Biome {
  public:
    PlainsBiome() : Biome("Plains", {-0.2f, 0.4f}, {-0.3f, 0.4f}, {-0.3f, 0.5f}) {
        auto& registry = RegistryManager::Blocks();

        m_descriptor.heightScale = 4.0f;
        m_descriptor.heightBase = 64.0f;


        // m_descriptor.layers.push_back(
        //     {.palette = {{0, registry.get("grass_deco")->getID()}, {50, 1}},
        //      .depth = -1,
        //      .features = {
        //          //new TreeFeature(registry.get("tree")->getID())
        //      }}
        // );

        m_descriptor.layers.push_back(
            {.palette = {{registry.get("grass")->getID()}, {1}}, .depth = 1}
        );
        m_descriptor.layers.push_back(
            {.palette = {{registry.get("dirt")->getID()}, {1}}, .depth = 4}
        );
        m_descriptor.layers.push_back(
            {.palette = {{registry.get("stone")->getID()}, {1}}, .depth = 100}
        );
    }


    // BlockID getBlockAt(const glm::ivec3& pos, int surfaceHeight) const override {}
};