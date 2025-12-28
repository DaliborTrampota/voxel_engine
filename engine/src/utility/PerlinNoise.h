#pragma once

#include <tools/PerlinNoise.h>

namespace engine {

    class PerlinNoise {
      public:
        PerlinNoise(
            siv::PerlinNoise::seed_type seed,
            float scale,
            float offset,
            int octaves = 1,
            float persistence = 0.5f
            // float lacunarity = 2.0f
        );

        float get(float x, float y) const noexcept;
        float getNormalized(float x, float y) const noexcept;
        float getClamped(float x, float y) const noexcept;

        float get1D(float x) const noexcept;
        float get1DNormalized(float x) const noexcept;
        float get1DClamped(float x) const noexcept;

        float get3D(float x, float y, float z) const noexcept;
        float get3DNormalized(float x, float y, float z) const noexcept;
        float get3DClamped(float x, float y, float z) const noexcept;


      private:
        siv::BasicPerlinNoise<float> m_noise;
        float m_scale;
        float m_offset;
        int m_octaves;
        float m_persistence;
        float m_lacunarity = 1.0f;
    };

}  // namespace engine