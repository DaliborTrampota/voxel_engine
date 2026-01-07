#include "PerlinNoise.h"

using namespace engine;


PerlinNoise::PerlinNoise(
    siv::PerlinNoise::seed_type seed, float scale, float offset, int octaves, float persistence
)
    : m_noise(seed),
      m_scale(1.0f / scale),
      m_offset(offset),
      m_octaves(octaves),
      m_persistence(persistence) {}


float PerlinNoise::get(float x, float y) const noexcept {
    float sampleX = x * m_scale;
    float sampleY = y * m_scale;
    return m_noise.octave2D(sampleX + m_offset, sampleY + m_offset, m_octaves, m_persistence);
}

float PerlinNoise::getNormalized(float x, float y) const noexcept {
    float sampleX = x * m_scale;
    float sampleY = y * m_scale;
    return m_noise.normalizedOctave2D(
        sampleX + m_offset, sampleY + m_offset, m_octaves, m_persistence
    );
}

float PerlinNoise::getClamped(float x, float y) const noexcept {
    float sampleX = x * m_scale;
    float sampleY = y * m_scale;
    return m_noise.octave2D_11(sampleX + m_offset, sampleY + m_offset, m_octaves, m_persistence);
}

float PerlinNoise::get1D(float x) const noexcept {
    return m_noise.octave1D(x + m_offset, m_octaves, m_persistence);
}

float PerlinNoise::get1DNormalized(float x) const noexcept {
    return m_noise.normalizedOctave1D(x + m_offset, m_octaves, m_persistence);
}

float PerlinNoise::get1DClamped(float x) const noexcept {
    return m_noise.octave1D_11(x + m_offset, m_octaves, m_persistence);
}

float PerlinNoise::get3D(float x, float y, float z) const noexcept {
    return m_noise.octave3D(x + m_offset, y + m_offset, z + m_offset, m_octaves, m_persistence);
}

float PerlinNoise::get3DNormalized(float x, float y, float z) const noexcept {
    return m_noise.normalizedOctave3D(
        x + m_offset, y + m_offset, z + m_offset, m_octaves, m_persistence
    );
}

float PerlinNoise::get3DClamped(float x, float y, float z) const noexcept {
    return m_noise.octave3D_11(x + m_offset, y + m_offset, z + m_offset, m_octaves, m_persistence);
}