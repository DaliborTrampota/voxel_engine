#pragma once

#include <utility/ThreadPool.h>
#include <memory>
#include "ISerializer.h"


namespace engine {
    class Chunk;
}

class WorldSerializer {
  public:
    WorldSerializer();

    void serialize(std::shared_ptr<const engine::Chunk> chunk);
    void deserialize(std::shared_ptr<engine::Chunk> chunk);
};