// clang-format off
#define CLUSTER_SIZE_X {{OMNI_CLUSTER_SIZE_X}}
#define CLUSTER_SIZE_Y {{OMNI_CLUSTER_SIZE_Y}}
#define CLUSTER_SIZE_Z {{OMNI_CLUSTER_SIZE_Z}}
// clang-format on

ivec3 clusterIndex() {
    int x = int(gl_FragCoord.x / resolution.x * CLUSTER_SIZE_X);
    int y = int(gl_FragCoord.y / resolution.y * CLUSTER_SIZE_Y);

    float viewDepth = max(abs((view * vec4(fragPos, 1.0)).z), nearPlane);
    int z = int(log(viewDepth / nearPlane) / log(farPlane / nearPlane) * CLUSTER_SIZE_Z);

    x = clamp(x, 0, CLUSTER_SIZE_X - 1);
    y = clamp(y, 0, CLUSTER_SIZE_Y - 1);
    z = clamp(z, 0, CLUSTER_SIZE_Z - 1);

    return ivec3(x, y, z);
}


float samplePointShadowMap(uint shadowIndex, vec3 lightToFragDir) {
    return texture(omniShadowMaps, vec4(lightToFragDir, shadowIndex)).r;
}

vec3 calculatePointLights(vec4 baseColor, ivec3 indexOfCluster) {
    vec3 result = vec3(0.0);

    uint index = indexOfCluster.x + indexOfCluster.y * CLUSTER_SIZE_X +
                 indexOfCluster.z * CLUSTER_SIZE_X * CLUSTER_SIZE_Y;
    ClusterGrid cluster = clusterGrid[index];
    if (cluster.count == 0) {
        return result;
    }

    for (uint i = 0; i < cluster.count; i++) {
        PointLight light = pointLight[lightIndices[cluster.offset + i]];

        vec3 toLight = light.position - fragPos;
        float dist = length(toLight);
        if (dist * dist > light.radius * light.radius)
            continue;

        float attenuation = 1.0 / (1.0 + dist + dist * dist);
        float diff = max(dot(normal, normalize(toLight)), 0.0);

        bool inShadow = false;
        if (light.shadowIndex != NoShadow) {
            float closestDepth = samplePointShadowMap(light.shadowIndex, fragPos - light.position);
            float currentDepth = length(fragPos - light.position) / light.radius;
            vec3 lightDir = normalize(toLight);
            float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.001);
            inShadow = currentDepth - bias > closestDepth;
        }
        if (inShadow) {
            continue;
        }
        result += light.color.rgb * light.intensity * diff * attenuation;
    }

    return result * baseColor.rgb;
}