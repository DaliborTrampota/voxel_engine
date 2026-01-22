#version 460 core

layout(std140) uniform LightSpaceMatrices {
    mat4 lightSpaceMatrices[4];
};

// uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 lightDir;
uniform vec3 viewPos;
uniform vec2 resolution;

out vec4 FragColor;
in vec3 pos;
in vec3 normal;
in vec2 uv;
in vec3 fragPos;

flat in uint texID;
in float ao;

uniform float time;
uniform mat4 view;


uniform sampler2DArray blockTextures;
uniform sampler2DArray shadowMap;
// uniform sampler2D translucentDepthMap;

// Volumetric translucency: density coefficient (0 = not translucent, >0 = translucency density)
uniform float translucentDensity;

const bool ENABLE_VSM = false;
const float C = 45.0;

const int cascadeCount = 4;
uniform float cascadePlaneDistances[cascadeCount];

float linstep(float low, float high, float v) {
    return clamp((v - low) / (high - low), 0.0, 1.0);
}

vec3 phongLighting(vec4 baseColor, float shadow) {
    vec3 ambientCol = 0.3 * lightColor;
    // vec3 fragLightDir = normalize(lightPos - fragPos);
    float diff = max(dot(-lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    //specular
    // vec3 viewDir = normalize(viewPos - fragPos.xyz);
    // float spec = 0.0;
    // vec3 halfwayDir = normalize(fragLightDir + viewDir);
    // spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = vec3(0.0);  // spec * lightColor;

    return (ambientCol + (1.0 - shadow) * (diffuse + specular)) * baseColor.rgb;
}

float shadowFaceOutside() {
    return dot(normal, -lightDir) > 0.0 ? 1.0 : 0.0;
}


float VSM(vec3 projCoords, float currentDepth, int layer) {
    vec2 moments = texture(shadowMap, vec3(projCoords.xy, layer)).rg;

    // if the current fragment is before the light occluder
    if (currentDepth <= moments.x)
        return 0.0;

    float variance = moments.y - (moments.x * moments.x);
    variance = max(variance, 0.00002);  // div by zero

    // Calculate probabilistic upper bound using Chebyshev's inequality
    float d = currentDepth - moments.x;
    float p_max = variance / (variance + d * d);

    // Reduce light bleeding (lower = softer, more bleeding)
    float lightBleedReduction = 0.35;
    p_max = linstep(lightBleedReduction, 1.0, p_max);

    return 1.0 - p_max;
}


float ShadowCalculation(vec3 fragPosWorldSpace, vec3 lightDirection) {
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

    if (ENABLE_VSM) {
        return VSM(projCoords, currentDepth, layer);
    } else {
        // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
        float closestDepth = texture(shadowMap, vec3(projCoords.xy, layer)).r;
        float bias = max(0.001 * (1.0 - dot(normal, -lightDirection)), 0.0001);
        return currentDepth - bias > closestDepth ? 1.0 : 0.0;
    }
}

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


void main() {
    vec4 col = texture(blockTextures, vec3(uv, texID));
    // vec3 normal = normalize(normal);
    // if (texID == 7u) {  // TODO grass coloring
    //     col.rgb *= vec3(0.4, 0.9, 0.3);
    // }

    float shadow = ShadowCalculation(fragPos, lightDir);
    vec3 lighting = phongLighting(col, shadow);

    float transmittance = 1.0;
    // if (translucentDensity > 0.0) {
    //     float backDepth = texture(translucentDepthMap, gl_FragCoord.xy / resolution).r;
    //     float frontDepth = gl_FragCoord.z;
    //     float thickness = backDepth - frontDepth;
    //     transmittance = exp(-translucentDensity * thickness);
    // }

    // subsurface scattering
    // vec3 scatterColor = vec3(1.0, 0.8, 0.6);
    // float scatter = 1.0 - exp(-translucentDensity * thickness * 0.5);
    // lighting = mix(lighting, lighting * scatterColor, scatter);

    FragColor = vec4(lighting, col.a) * transmittance;
}