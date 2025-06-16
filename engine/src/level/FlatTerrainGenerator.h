#pragma once

#include <glm/glm.hpp>

#include "ITerrainGenerator.h"

#include "block/Block.h"
#include "data/Registry.h"

namespace engine {

    class FlatTerrainGenerator : public ITerrainGenerator {
      public:
        FlatTerrainGenerator();

        BlockID voxelAt(const glm::ivec3& pos) override;
        void populate(Chunk& chunk) override;

        BlockID voxelAt(const glm::ivec3& pos, int height);
        int height(int x, int z) const;

      protected:
        const Registry<Block>& m_blockRegistry;
    };
}  // namespace engine