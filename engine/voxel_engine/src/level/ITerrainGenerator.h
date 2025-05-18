#pragma once

#include <glm/glm.hpp>

#include "../Globals.h"

namespace engine {
    class Chunk;

    class ITerrainGenerator {
      public:
        virtual ~ITerrainGenerator() = default;

        virtual BlockID voxelAt(const glm::ivec3& pos) = 0;
        virtual void populate(Chunk& chunk) = 0;

      protected:
        /// This should be used as an optimization, when populating chunk terrain,
        /// usually some kind of noise is used which determines the height of the terrain.
        /// This height does not change for xz coordinates so it's unnecessary to compute it
        /// over and over for each voxel column. You should obtain somewhere the height and then
        /// pass that height as y param.
        //virtual BlockID voxelAt(const glm::ivec3& pos, int y) {
        //    return -1;
        //};
    };
}  // namespace engine