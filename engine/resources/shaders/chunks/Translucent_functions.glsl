
// Calculate volumetric translucency opacity based on depth through the block
// Uses exponential falloff: opacity = 1 - exp(-density * depth)
float calculateTranslucentOpacity(vec3 fragPos) {
    if (translucentDensity <= 0.0) {
        return 1.0;  // Not translucent, fully opaque
    }

    // Calculate block center (assuming 1x1x1 blocks, center is at .5 offset from integer position)
    vec3 blockCenter = floor(fragPos) + 0.5;

    // Calculate view direction FROM camera TO fragment
    vec3 viewDir = normalize(fragPos - viewPos);

    // Calculate entry point of ray into the block
    // For a cube centered at blockCenter with size 1, find intersection with view ray
    vec3 rayOrigin = viewPos;
    vec3 rayDir = viewDir;

    // Ray-box intersection to find entry point
    vec3 blockMin = blockCenter - 0.5;
    vec3 blockMax = blockCenter + 0.5;

    vec3 invDir = 1.0 / rayDir;
    vec3 t1 = (blockMin - rayOrigin) * invDir;
    vec3 t2 = (blockMax - rayOrigin) * invDir;

    vec3 tMin = min(t1, t2);
    vec3 tMax = max(t1, t2);

    float entryDist = max(max(tMin.x, tMin.y), tMin.z);
    float exitDist = min(min(tMax.x, tMax.y), tMax.z);

    // If no intersection, return fully opaque
    if (entryDist > exitDist || exitDist < 0.0) {
        return 1.0;
    }

    // Calculate depth through block from entry point to fragment
    vec3 entryPoint = rayOrigin + rayDir * entryDist;
    float depth = length(fragPos - entryPoint);

    // Clamp depth to block size (1.0) to avoid artifacts
    depth = min(depth, 1.0);

    // Apply exponential falloff (Beer-Lambert law)
    // Higher density = more opaque per unit depth
    float opacity = 1.0 - exp(-translucentDensity * depth);

    return opacity;
}
