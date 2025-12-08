#include "RegistryManager.h"

#include "Registry.h"
#include "block/Block.h"
#include "block/Geometry.h"


namespace engine {
    Registry<Block, RegistryStoragePolicy::ByPointer>& RegistryManager::Blocks() {
        static Registry<Block, RegistryStoragePolicy::ByPointer> s_instance;
        return s_instance;
    }

    Registry<Geometry>& RegistryManager::Geometries() {
        static Registry<Geometry> s_instance;
        return s_instance;
    }
}  // namespace engine