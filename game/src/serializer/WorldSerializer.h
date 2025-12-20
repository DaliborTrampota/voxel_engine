#pragma once

#include "ISerializer.h"

namespace engine {
    class Chunk;
}

class WorldSerializer {
  public:
    void serialize(const engine::Chunk& chunk);
    void deserialize(engine::Chunk& chunk);
};