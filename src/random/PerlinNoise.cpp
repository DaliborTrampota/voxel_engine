#include "PerlinNoise.h"
// todo index.h usage?
#include <cassert>
#include <vector>


using namespace engine;


PerlinNoise::PerlinNoise(
    PerlinNoise::Seed seed,
    float scale,
    float offset,
    uint32_t octaves,
    float persistence,
    float lacunarity
)
    : m_noise(FastNoise::New<FastNoise::FractalFBm>()),
      m_seed(seed),
      m_offset(offset),
      m_octaves(octaves > 0 ? octaves : 1u) {
    auto perlin = FastNoise::New<FastNoise::Simplex>();
    perlin->SetScale(scale);
    m_noise->SetSource(perlin);
    m_noise->SetOctaveCount(m_octaves);
    m_noise->SetGain(persistence);
    m_noise->SetLacunarity(lacunarity);
    m_maxAmplitude = MaxAmplitude(m_octaves, persistence);
}


float PerlinNoise::get2D(float x, float y) const noexcept {
    return m_noise->GenSingle2D(x + m_offset, y + m_offset, m_seed);
}

float PerlinNoise::get2DNormalized(float x, float y) const noexcept {
    return m_noise->GenSingle2D(x + m_offset, y + m_offset, m_seed) / m_maxAmplitude;
}

void PerlinNoise::genArea2D(float* out, glm::ivec2 start, glm::ivec2 dims) const noexcept {
    assert(dims.x > 0 && dims.y > 0);
    size_t count = static_cast<size_t>(dims.x) * static_cast<size_t>(dims.y);
    std::vector<float> positionsX(count);
    std::vector<float> positionsY(count);
    size_t index = 0;
    for (int y = start.y; y < start.y + dims.y; y++) {
        for (int x = start.x; x < start.x + dims.x; x++) {
            positionsX[index] = x;
            positionsY[index] = y;
            index++;
        }
    }
    m_noise->GenPositionArray2D(
        out, count, positionsX.data(), positionsY.data(), m_offset, m_offset, m_seed
    );
}


float PerlinNoise::get3D(float x, float y, float z) const noexcept {
    return m_noise->GenSingle3D(x + m_offset, y + m_offset, z + m_offset, m_seed);
}

float PerlinNoise::get3DNormalized(float x, float y, float z) const noexcept {
    return m_noise->GenSingle3D(x + m_offset, y + m_offset, z + m_offset, m_seed) / m_maxAmplitude;
}

void PerlinNoise::genArea3D(float* out, glm::ivec3 start, glm::ivec3 dims) const noexcept {
    assert(dims.x > 0 && dims.y > 0 && dims.z > 0);
    size_t count =
        static_cast<size_t>(dims.x) * static_cast<size_t>(dims.y) * static_cast<size_t>(dims.z);
    std::vector<float> positionsX(count);
    std::vector<float> positionsY(count);
    std::vector<float> positionsZ(count);
    size_t index = 0;
    for (int z = start.z; z < start.z + dims.z; z++) {
        for (int y = start.y; y < start.y + dims.y; y++) {
            for (int x = start.x; x < start.x + dims.x; x++) {
                positionsX[index] = x;
                positionsY[index] = y;
                positionsZ[index] = z;
                index++;
            }
        }
    }
    m_noise->GenPositionArray3D(
        out,
        count,
        positionsX.data(),
        positionsY.data(),
        positionsZ.data(),
        m_offset,
        m_offset,
        m_offset,
        m_seed
    );
}

float PerlinNoise::get4D(float x, float y, float z, float w) const noexcept {
    return m_noise->GenSingle4D(x + m_offset, y + m_offset, z + m_offset, w + m_offset, m_seed);
}

float PerlinNoise::get4DNormalized(float x, float y, float z, float w) const noexcept {
    return m_noise->GenSingle4D(x + m_offset, y + m_offset, z + m_offset, w + m_offset, m_seed) /
           m_maxAmplitude;
}

void PerlinNoise::genArea4D(float* out, glm::ivec4 start, glm::ivec4 dims) const noexcept {
    assert(dims.x > 0 && dims.y > 0 && dims.z > 0 && dims.w > 0);
    size_t count = static_cast<size_t>(dims.x) * static_cast<size_t>(dims.y) *
                   static_cast<size_t>(dims.z) * static_cast<size_t>(dims.w);
    std::vector<float> positionsX(count);
    std::vector<float> positionsY(count);
    std::vector<float> positionsZ(count);
    std::vector<float> positionsW(count);
    size_t index = 0;
    for (int w = start.w; w < start.w + dims.w; w++) {
        for (int z = start.z; z < start.z + dims.z; z++) {
            for (int y = start.y; y < start.y + dims.y; y++) {
                for (int x = start.x; x < start.x + dims.x; x++) {
                    positionsX[index] = x;
                    positionsY[index] = y;
                    positionsZ[index] = z;
                    positionsW[index] = w;
                    index++;
                }
            }
        }
    }

    m_noise->GenPositionArray4D(
        out,
        count,
        positionsX.data(),
        positionsY.data(),
        positionsZ.data(),
        positionsW.data(),
        m_offset,
        m_offset,
        m_offset,
        m_offset,
        m_seed
    );
}

void PerlinNoise::genPositions4D(
    float* out, const float* inX, const float* inY, const float* inZ, const float* inW, size_t count
) const noexcept {
    m_noise->GenPositionArray4D(
        out, count, inX, inY, inZ, inW, m_offset, m_offset, m_offset, m_offset, m_seed
    );
}