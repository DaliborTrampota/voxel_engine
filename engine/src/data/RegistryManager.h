#pragma once

#include "Registry.h"
#include "block/Block.h"
#include "block/Geometry.h"


namespace engine {
    class RegistryManager {
      public:
        static Registry<Block, RegistryStoragePolicy::ByPointer>& Blocks();
        static Registry<Geometry>& Geometries();
    };
}  // namespace engine