
struct PointLight {
    vec3 position;   // offset 0  (GPU inserts 4 bytes implicit padding after, color starts at 16)
    vec3 color;      // offset 16 (GPU inserts no padding after, intensity starts at 28)
    float intensity; // offset 28
    float radius;    // offset 32
    uint shadowIndex;// offset 36
    // 8 bytes implicit trailing padding, stride = 48
};

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
