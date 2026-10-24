// Sea-surface displacement shared by ChunkVert.glsl and SunVert.glsl. Stomata's WaterTests compile this file
// as C++ (uniform -> static), so keep it to syntax valid in both.
const uint SEA_SURFACE_BIT = 1u << 18;
const uint SHORE_SKIRT_BIT = 1u << 19;
const int WAVE_CAPACITY = 48;
const int WAVE_PATCH_CAPACITY = 4;

uniform float tideLevel;
uniform vec2 worldPeriod;
uniform int waveCount;
uniform vec2 waveK[WAVE_CAPACITY];
uniform float waveAmp[WAVE_CAPACITY];
uniform float wavePhase[WAVE_CAPACITY];
uniform int wavePatchCount;
uniform vec2 wavePatchK[WAVE_PATCH_CAPACITY];
uniform float wavePatchPhase[WAVE_PATCH_CAPACITY];
uniform float wavePatchBase;
uniform float wavePatchDepth;

/// x = height, yz = (dh/dx, dh/dz). The sum of Stomata's WaveField::heightAt, with xz reduced modulo the period.
vec3 waveHeightGrad(vec2 xz) {
    if (worldPeriod.x > 0.0f && worldPeriod.y > 0.0f)
        xz = mod(xz, worldPeriod);

    float sum = 0.0f;
    vec2 sumGrad = vec2(0.0f);
    int n = min(waveCount, WAVE_CAPACITY);
    for (int i = 0; i < n; ++i) {
        float a = dot(waveK[i], xz) - wavePhase[i];
        sum += waveAmp[i] * sin(a);
        sumGrad += (waveAmp[i] * cos(a)) * waveK[i];
    }

    float patchSum = 0.0f;
    vec2 patchGrad = vec2(0.0f);
    int m = min(wavePatchCount, WAVE_PATCH_CAPACITY);
    for (int j = 0; j < m; ++j) {
        float a = dot(wavePatchK[j], xz) - wavePatchPhase[j];
        patchSum += sin(a);
        patchGrad += cos(a) * wavePatchK[j];
    }
    float depth = m > 0 ? wavePatchDepth / float(m) : 0.0f;
    float patchScale = wavePatchBase + depth * patchSum;

    return vec3(patchScale * sum, patchScale * sumGrad + (sum * depth) * patchGrad);
}

/// Final y of a SEA_SURFACE_BIT vertex, which is emitted at its cell top: the tide within that cell plus waves.
/// A side face's moved edge never drops below its bottom edge: the cell floor, or the cell top on a shoreline skirt.
float seaSurfaceY(float cellTop, vec3 waves, uint data, bool sideFace) {
    float y = clamp(tideLevel, cellTop - 1.0f, cellTop) + waves.x;
    if (sideFace)
        y = max(y, (data & SHORE_SKIRT_BIT) != 0u ? cellTop : cellTop - 1.0f);
    return y;
}
