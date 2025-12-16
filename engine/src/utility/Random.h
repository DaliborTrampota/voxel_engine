#pragma once

#include <cstdint>

namespace engine {

    /**
 * Deterministic random number generator based on coordinates.
 * Returns the same value for the same coordinates, regardless of access order.
 */
    class Random {
      public:
        static uint32_t hash2D(int x, int y);
        static uint32_t hash3D(int x, int y, int z);

        /// @returns Value between -1 and 1
        static float noise2D(int x, int y);
        static float noise3D(int x, int y, int z);

        /// @returns Value between [0 and 1)
        static float random2D(int x, int y);
        static float random3D(int x, int y, int z);

        /// @returns Value between [min and max)
        static int randomRange2D(int x, int y, int min, int max);
        static int randomRange3D(int x, int y, int z, int min, int max);

      private:
        static uint32_t hash(uint32_t x);
    };

}  // namespace engine