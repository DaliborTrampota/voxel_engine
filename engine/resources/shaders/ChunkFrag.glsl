#version 330 core

out vec4 FragColor;
in vec3 pos;
in vec3 normal;
in vec2 uv;

in ShadowData {
    vec3 fragPos;
    vec4 fragPosLightSpace;
}
shadowData;

flat in uint texID;
in float ao;

uniform sampler2DArray texArray;
uniform float time;

//TODO UBO?
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 lightDir;
uniform vec3 viewPos;
uniform sampler2D shadowMap;

bool ENABLE_VSM = true;
float C = 45.0;


float linstep(float low, float high, float v) {
    return clamp((v - low) / (high - low), 0.0, 1.0);
}

vec3 phongLighting(vec4 baseColor, float shadow) {
    vec3 ambientCol = 0.3 * lightColor;
    vec3 fragLightDir = normalize(lightPos - shadowData.fragPos);
    float diff = max(dot(fragLightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    //specular
    // vec3 viewDir = normalize(viewPos - shadowData.fragPos.xyz);
    // float spec = 0.0;
    // vec3 halfwayDir = normalize(fragLightDir + viewDir);
    // spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = vec3(0.0);  // spec * lightColor;

    return (ambientCol + (1.0 - shadow) * (diffuse + specular)) * baseColor.rgb;
}

float shadowFaceOutside() {
    return dot(normal, lightDir) > 0.0 ? 1.0 : 0.0;
}


float VSM(vec3 projCoords, float currentDepth) {
    // Read the moments from the VSM shadow map
    vec2 moments = texture(shadowMap, projCoords.xy).rg;

    // Surface is fully lit if the current fragment is before the light occluder
    if (currentDepth <= moments.x)
        return 0.0;

    // Calculate variance
    float variance = moments.y - (moments.x * moments.x);
    variance = max(variance, 0.00002);  // Minimum variance to avoid division by zero

    // Calculate probabilistic upper bound using Chebyshev's inequality
    float d = currentDepth - moments.x;
    float p_max = variance / (variance + d * d);

    // Reduce light bleeding (lower = softer, more bleeding)
    float lightBleedReduction = 0.2;
    p_max = linstep(lightBleedReduction, 1.0, p_max);

    return 1.0 - p_max;
}

float EVMS(vec3 projCoords, float currentDepth) {
    vec4 moments = texture(shadowMap, projCoords.xy);

    float e1 = exp(+C * currentDepth);
    float e3 = exp(-C * currentDepth);

    float pos_mean = moments.x;
    float pos_mean_sq = moments.y;
    float neg_mean = moments.z;
    float neg_mean_sq = moments.w;

    if (e1 <= pos_mean && e3 >= neg_mean)
        return 0.0;

    float e2 = exp(+2.0 * C * currentDepth);
    float e4 = exp(-2.0 * C * currentDepth);

    float p_max_plus =
        (moments.y - moments.x * moments.x) / (moments.y - 2.0 * moments.x * e1 + e2);

    float p_max_minus =
        (moments.w - moments.z * moments.z) / (moments.w - 2.0 * moments.z * e3 + e4);

    // Take the minimum probability (most conservative)
    float p = clamp(min(p_max_plus, p_max_minus), 0.0, 1.0);

    // Light bleed reduction
    float bleedReduce = 0.2;
    p = linstep(bleedReduce, 1.0, p);

    return 1.0 - p;
}


float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDirection) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0 ||
        projCoords.z < 0.0 || projCoords.z > 1.0) {
        return shadowFaceOutside();
    }

    float currentDepth = projCoords.z;

    if (ENABLE_VSM) {
        // return VSM(projCoords, currentDepth);
        return EVMS(projCoords, currentDepth);
    } else {
        // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
        float closestDepth = texture(shadowMap, projCoords.xy).r;
        float bias = max(0.005 * (1.0 - dot(normal, lightDirection)), 0.0005);
        return currentDepth - bias > closestDepth ? 1.0 : 0.0;
    }
}


void main() {
    vec4 col = texture(texArray, vec3(uv, texID));
    // vec3 normal = normalize(normal);
    if (texID == 7u) {  // TODO grass coloring
        col.rgb *= vec3(0.4, 0.9, 0.3);
    }

    float shadow = ShadowCalculation(shadowData.fragPosLightSpace, lightDir);
    FragColor = vec4(phongLighting(col, shadow), col.a);
}