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


      private:
        siv::BasicPerlinNoise<float> m_noise;
        float m_scale;
        float m_offset;
        int m_octaves;
        float m_persistence;
        float m_lacunarity = 1.0f;
    };

}  // namespace engine