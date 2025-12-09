#include "RegistryManager.h"

#include "Registry.h"

namespace engine {
    RegistryManager::BlockRegistryT& RegistryManager::Blocks() {
        static std::decay_t<BlockRegistryT> s_instance;
        return s_instance;
    }

    RegistryManager::GeometryRegistryT& RegistryManager::Geometries() {
        static std::decay_t<GeometryRegistryT> s_instance;
        return s_instance;
    }
}  // namespace engine