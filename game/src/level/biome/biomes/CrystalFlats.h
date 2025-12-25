#pragma once

#include "../Biome.h"

#include <data/RegistryManager.h>

class CrystalFlatsBiome : public Biome {
  public:
    CrystalFlatsBiome()
        : Biome("CrystalFlats", {0.4f, 1.0f}, {0.3f, 1.0f}, ParameterRange::Low, 80) {
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
            {.palette = {{registry.get("crystal_crust")->getID()}, {1}}, .depth = 4}
        );
        m_descriptor.layers.push_back(
            {.palette = {{registry.get("slate")->getID()}, {1}}, .depth = -1}
        );
    }


    // BlockID getBlockAt(const glm::ivec3& pos, int surfaceHeight) const override {}
};