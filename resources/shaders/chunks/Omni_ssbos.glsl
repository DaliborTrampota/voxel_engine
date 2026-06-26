
struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float radius;
    uint shadowIndex;
    // 8 bytes implicit trailing padding, stride = 48
};

const uint NoShadow = 0xFFFFFFFF;

struct ClusterGrid {
    uint offset;
    uint count;
};

layout(std430, binding = 1) readonly buffer lightSSBO {
    PointLight pointLight[];
};

layout(std430, binding = 2) readonly buffer clusterGridSSBO {
    ClusterGrid clusterGrid[];
};

layout(std430, binding = 3) readonly buffer lightIndicesSSBO {
    uint lightIndices[];
};

uniform samplerCubeArray omniShadowMaps;
uniform bool omniLightsEnabled;