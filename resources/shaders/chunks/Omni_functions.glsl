ivec3 clusterIndex() {
    int x = int(gl_FragCoord.x / resolution.x * 16);
    int y = int(gl_FragCoord.y / resolution.y * 9);

    float viewDepth = (view * vec4(fragPos, 1.0)).z;
    int z = int(log(abs(viewDepth) / nearPlane) / log(farPlane / nearPlane) * 24);

    x = clamp(x, 0, 15);
    y = clamp(y, 0, 8);
    z = clamp(z, 0, 23);

    return ivec3(x, y, z);
}

uniform uint lightCount;

vec3 calculatePointLights(vec4 baseColor, ivec3 indexOfCluster) {
    vec3 result = vec3(0.0);

    for (uint i = 0; i < lightCount; i++) {
        PointLight light = pointLight[i];

        vec3 toLight = light.position.xyz - fragPos;
        float dist = length(toLight);
        if (dist * dist > light.radius * light.radius)
            continue;
        float attenuation = 1.0 / (1.0 + dist + dist * dist);
        float diff = max(dot(normal, normalize(toLight)), 0.0);

        result += light.color.rgb * light.intensity * diff * attenuation;
    }

    return result * baseColor.rgb;
}