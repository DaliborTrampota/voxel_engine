#pragma once

#include <cstdint>
#include <random>

namespace {
    inline std::random_device s_randomDevice;
    inline std::mt19937 s_randomEngine(s_randomDevice());
}  // namespace

namespace engine {

    /**
 * Deterministic random number generator based on coordinates.
 * Returns the same value for the same coordinates, regardless of access order.
 */
    class Random {
      public:
        static uint32_t hash2D(int x, int y);
        static uint32_t hash3D(int x, int y, int z);

        /// @returns Random but same for the same coordinates, value between -1 and 1
        static float noise2D(int x, int y);
        static float noise3D(int x, int y, int z);

        /// @returns Random but same for the same coordinates, value between [0 and 1)
        static float random2D(int x, int y);
        static float random3D(int x, int y, int z);

        /// @returns Random but same for the same coordinates, value between [min and max)
        static int randomRange2D(int x, int y, int min, int max);
        static int randomRange3D(int x, int y, int z, int min, int max);

        /// @returns Value between [min and max)
        template <typename T>
        static T random(T min = 0.0, T max = 1.0) {
            if constexpr (std::is_integral_v<T>) {
                return std::uniform_int_distribution<T>(min, max)(s_randomEngine);
            } else {
                return std::uniform_real_distribution<T>(min, max)(s_randomEngine);
            }
        }

      private:
        static uint32_t hash(uint32_t x);
    };

}  // namespace engine