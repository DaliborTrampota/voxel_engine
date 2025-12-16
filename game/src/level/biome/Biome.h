#pragma once

#include <string>

#include <block/Block.h>
#include <glm/glm.hpp>

#include "BiomeDescriptor.h"


using namespace engine;

struct Parameter {
    float min;
    float max;

    float distance(float val) const {
        float d1 = val - max;
        float d2 = min - val;
        return d1 > 0 ? d1 : glm::max(d2, 0.0f);
    }
};


class Biome {
  public:
    Biome(
        const std::string& name,
        Parameter temperature,
        Parameter concentration,
        Parameter altitude,
        float priority = 0
    )
        : m_name(name),
          m_temperature(temperature),
          m_concentration(concentration),
          m_altitude(altitude),
          m_priority(priority) {}

    const std::string& name() const { return m_name; }
    const Parameter& temperature() const { return m_temperature; }
    const Parameter& concentration() const { return m_concentration; }
    const Parameter& altitude() const { return m_altitude; }

    float fitScore(float temperature, float concentration, float altitude) const;
    int priority() const { return m_priority; }

    virtual float getHeightModifier(int x, int z, float baseHeight) const;
    virtual BlockID getBlockAt(const glm::ivec3& pos, int surfaceHeight) const;

    BiomeDescriptor& descriptor() { return m_descriptor; }
    const BiomeDescriptor& descriptor() const { return m_descriptor; }

  protected:
    std::string m_name;

    Parameter m_temperature;
    Parameter m_concentration;
    Parameter m_altitude;

    int m_priority = 0;

    BiomeDescriptor m_descriptor;
};