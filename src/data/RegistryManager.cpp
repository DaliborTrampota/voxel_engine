#include "RegistryManager.h"

#include "Registry.h"

namespace engine {
    RegistryManager::BlockRegistryT& RegistryManager::Blocks() {
        static BlockRegistryT s_instance;
        return s_instance;
    }

    RegistryManager::GeometryRegistryT& RegistryManager::Geometries() {
        static GeometryRegistryT s_instance;
        return s_instance;
    }
}  // namespace engine