
float shadowFaceOutside() {
    return dot(normal, lightDir) > 0.0 ? 1.0 : 0.0;
}

float calculateShadow(vec3 fragPosWorldSpace) {
    vec4 fragPosViewSpace = view * vec4(fragPosWorldSpace, 1.0);
    float depthValue = abs(fragPosViewSpace.z);

    // selecting cascade
    int layer = -1;
    for (int i = 0; i < cascadeCount; ++i) {
        if (depthValue < cascadePlaneDistances[i]) {
            layer = i;
            break;
        }
    }
    if (layer == -1) {
        layer = cascadeCount - 1;
    }


    vec4 fragPosLightSpace = lightSpaceMatrices[layer] * vec4(fragPosWorldSpace, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0 ||
        projCoords.z < 0.0 || projCoords.z > 1.0) {
        return shadowFaceOutside();
    }

    float currentDepth = projCoords.z;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, vec3(projCoords.xy, layer)).r;
    float bias = max(0.001 * (1.0 - dot(normal, lightDir)), 0.0001);
    return currentDepth - bias > closestDepth ? 1.0 : 0.0;
}
