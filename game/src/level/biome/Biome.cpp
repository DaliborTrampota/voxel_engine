#include "Biome.h"
#include <utility/Random.h>

using namespace engine;


float Biome::fitScore(float temperature, float concentration) const {
    return glm::sqrt(m_temperature.distance(temperature)) +
           glm::sqrt(m_concentration.distance(concentration));
}

float Biome::getHeightModifier(int x, int z, float baseHeight) const {
    return baseHeight * m_descriptor.heightScale + m_descriptor.heightBase;
}

BlockID Biome::getBlockAt(const glm::ivec3& pos, int surfaceHeight) const {
    int startY;
    int endY = surfaceHeight;
    for (const auto& layer : m_descriptor.layers) {
        startY = endY;
        if (layer.depth == -1) {
            endY = std::numeric_limits<int>::min();
        } else {
            endY = startY - layer.depth;
        }

        if (pos.y > startY)
            break;

        if (pos.y <= startY && pos.y > endY) {
            return layer.palette.get(pos);
        }
    }

    return 0;
}