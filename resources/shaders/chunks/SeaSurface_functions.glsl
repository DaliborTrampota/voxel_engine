// Sea-surface displacement shared by ChunkVert.glsl and SunVert.glsl. Host tests may compile this file as C++
// (uniform -> static), so keep it to syntax valid in both.
const uint SEA_SURFACE_BIT = 1u << 18;
const uint SHORE_SKIRT_BIT = 1u << 19;
const uint SHALLOW_CORNER_BIT = 1u << 20;
const float SHALLOW_SINK = 1.0f / 16.0f;
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

/// x = height, yz = (dh/dx, dh/dz) of patch(xz) * sum_i amp_i*sin(dot(k_i, xz) - phase_i), xz reduced modulo the period.
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

/// waveHeightGrad at a SEA_SURFACE_BIT vertex; at a SHALLOW_CORNER_BIT one it is scaled by the tide's height above
/// the vertex's cell floor, so waves reach full height only with a whole water block below.
vec3 seaSurfaceWaves(vec2 xz, float cellTop, uint data) {
    vec3 waves = waveHeightGrad(xz);
    if ((data & SHALLOW_CORNER_BIT) != 0u)
        waves *= clamp(tideLevel, cellTop - 1.0f, cellTop) - (cellTop - 1.0f);
    return waves;
}

/// Final y of a SEA_SURFACE_BIT vertex, which is emitted at its cell top: the tide within that cell plus waves.
/// tideLevel moves in whole liquid units. Side faces stop at the cell floor; a shallow corner with the tide at or below
/// the land top sinks SHALLOW_SINK under it so the land hides it. A shoreline skirt's moved edge stops at the cell top.
float seaSurfaceY(float cellTop, vec3 waves, uint data, bool sideFace) {
    float floorY = cellTop - 1.0f;
    float y = clamp(tideLevel, floorY, cellTop) + waves.x;
    if ((data & SHALLOW_CORNER_BIT) != 0u && tideLevel <= floorY + 1e-3f)
        y = floorY - SHALLOW_SINK;
    else if (sideFace || (data & SHALLOW_CORNER_BIT) != 0u)
        y = max(y, floorY);
    if (sideFace && (data & SHORE_SKIRT_BIT) != 0u)
        y = max(y, cellTop);
    return y;
}
