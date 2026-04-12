#include "Random.h"

namespace engine {

    // Internal hash function using bit mixing
    uint32_t Random::hash(uint32_t x) {
        x = ((x >> 16) ^ x) * 0x45d9f3b;
        x = ((x >> 16) ^ x) * 0x45d9f3b;
        x = (x >> 16) ^ x;
        return x;
    }

    // Hash 2D coordinates into a single value
    uint32_t Random::hash2D(int x, int y) {
        // Mix coordinates using prime numbers
        uint32_t h = 0x9e3779b9;  // Golden ratio constant
        h ^= static_cast<uint32_t>(x) * 0x1b873593;
        h = (h << 13) | (h >> 19);  // Rotate left 13
        h ^= static_cast<uint32_t>(y) * 0xcc9e2d51;
        return hash(h);
    }

    // Hash 3D coordinates into a single value
    uint32_t Random::hash3D(int x, int y, int z) {
        // Mix coordinates using prime numbers
        uint32_t h = 0x9e3779b9;  // Golden ratio constant
        h ^= static_cast<uint32_t>(x) * 0x1b873593;
        h = (h << 13) | (h >> 19);  // Rotate left 13
        h ^= static_cast<uint32_t>(y) * 0xcc9e2d51;
        h = (h << 13) | (h >> 19);  // Rotate left 13
        h ^= static_cast<uint32_t>(z) * 0x85ebca6b;
        return hash(h);
    }

    float Random::random2D(int x, int y) {
        uint32_t h = hash2D(x, y);
        return static_cast<float>(h) / static_cast<float>(UINT32_MAX);
    }

    float Random::random3D(int x, int y, int z) {
        uint32_t h = hash3D(x, y, z);
        return static_cast<float>(h) / static_cast<float>(UINT32_MAX);
    }

    float Random::noise2D(int x, int y) {
        return random2D(x, y) * 2.0f - 1.0f;
    }

    float Random::noise3D(int x, int y, int z) {
        return random3D(x, y, z) * 2.0f - 1.0f;
    }

    int Random::randomRange2D(int x, int y, int min, int max) {
        if (min > max) {
            int temp = min;
            min = max;
            max = temp;
        }
        uint32_t h = hash2D(x, y);
        return min + static_cast<int>(h % static_cast<uint32_t>(max - min + 1));
    }

    int Random::randomRange3D(int x, int y, int z, int min, int max) {
        if (min > max) {
            int temp = min;
            min = max;
            max = temp;
        }
        uint32_t h = hash3D(x, y, z);
        return min + static_cast<int>(h % static_cast<uint32_t>(max - min + 1));
    }

}  // namespace engine