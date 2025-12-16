#pragma once


#include <glm/glm.hpp>
#include <numeric>
#include <stdexcept>
#include <vector>

#include <Globals.h>
#include <utility/Random.h>


using namespace engine;

struct Palette {
    std::vector<BlockID> blocks;
    std::vector<int> weights;
    int totalWeight;

    // Constructor that calculates totalWeight automatically
    Palette() : totalWeight(0) {}
    Palette(std::vector<BlockID> blocks_, std::vector<int> weights_)
        : blocks(std::move(blocks_)),
          weights(std::move(weights_)),
          totalWeight(std::accumulate(weights.begin(), weights.end(), 0)) {
        if (blocks.size() != weights.size()) {
            throw std::invalid_argument("blocks and weights must have the same size");
        }
    }

    BlockID get(const glm::ivec3& pos) const {
        float a = Random::random3D(pos.x, pos.y, pos.z) * totalWeight;

        int cursor = 0;
        for (int i = 0; i < weights.size(); i++) {
            cursor += weights[i];
            if (a < cursor) {
                return blocks[i];
            }
        }
        return blocks.back();  // Not possible
    }
};

struct Feature {
    float chance;
    virtual void operator()(const glm::ivec3& pos) const = 0;
};

struct TerrainLayer {
    Palette palette;
    int depth;
    std::vector<Feature*> features;
};


struct BiomeDescriptor {
    std::vector<TerrainLayer> layers;

    float heightScale;
    float heightBase;
};