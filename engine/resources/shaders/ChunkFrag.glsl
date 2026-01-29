#version 460 core

// clang-format off
{{CSM_uniforms.glsl}}
// clang-format on

const float ALPHA_THRESHOLD = 0.5;

out vec4 FragColor;
in vec3 pos;
in vec3 normal;
in vec2 uv;
in vec3 fragPos;

flat in uint texID;
in float ao;

uniform float time;


uniform sampler2DArray blockTextures;
uniform sampler2DArray shadowMap;
// uniform sampler2D translucentDepthMap;

// Volumetric translucency: density coefficient (0 = not translucent, >0 = translucency density)
uniform float translucentDensity;

// clang-format off
{{Utils_functions.glsl}}
// clang-format on

vec3 phongLighting(vec4 baseColor, float shadow) {
    vec3 ambientCol = 0.3 * lightColor;
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    //specular
    // vec3 viewDir = normalize(viewPos - fragPos.xyz);
    // float spec = 0.0;
    // vec3 halfwayDir = normalize(fragLightDir + viewDir);
    // spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = vec3(0.0);  // spec * lightColor;

    return (ambientCol + (1.0 - shadow) * (diffuse + specular)) * baseColor.rgb;
}

// clang-format off
{{CSM_functions.glsl}}
// clang-format on


void main() {
    vec4 col = texture(blockTextures, vec3(uv, texID));
    if (col.a < ALPHA_THRESHOLD) {
        discard;
    }


    float shadow = calculateShadow(fragPos);
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