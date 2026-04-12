#pragma once

#include <FastNoise/FastNoise.h>

#include <glm/glm.hpp>

#include <cstddef>
#include <cstdint>

namespace engine {

    class PerlinNoise {
      public:
        PerlinNoise(
            uint64_t seed,
            float scale,
            float offset,
            uint32_t octaves = 1,
            float persistence = 0.5f,
            float lacunarity = 2.0f
        );

        float get2D(float x, float y) const noexcept;
        float get2DNormalized(float x, float y) const noexcept;
        void genArea2D(float* out, glm::ivec2 start, glm::uvec2 dims) const noexcept;

        float get3D(float x, float y, float z) const noexcept;
        float get3DNormalized(float x, float y, float z) const noexcept;
        void genArea3D(float* out, glm::ivec3 start, glm::uvec3 dims) const noexcept;

        float get4D(float x, float y, float z, float w) const noexcept;
        float get4DNormalized(float x, float y, float z, float w) const noexcept;
        void genArea4D(float* out, glm::ivec4 start, glm::uvec4 dims) const noexcept;
        void genPositions4D(
            float* out,
            const float* inX,
            const float* inY,
            const float* inZ,
            const float* inW,
            size_t count
        ) const noexcept;

      private:
        FastNoise::SmartNode<FastNoise::FractalFBm> m_noise;
        uint64_t m_seed;
        float m_offset;
        uint32_t m_octaves;
        float m_persistence;

        float m_maxAmplitude;

        static constexpr float MaxAmplitude(
            const uint32_t octaves, const float persistence
        ) noexcept {
            float result = 0;
            float amplitude = 1;

            for (uint32_t i = 0; i < octaves; ++i) {
                result += amplitude;
                amplitude *= persistence;
            }

            return result;
        }
    };

}  // namespace engine