#version 460 core

layout(location = 0) out vec4 accum;
layout(location = 1) out float reveal;


// clang-format off
{{CSM_uniforms.glsl}}
// clang-format on

in vec3 pos;
in vec3 normal;
in vec2 uv;
in vec3 fragPos;

flat in uint texID;
in float ao;

uniform float time;

layout(binding = 0) uniform sampler2DArray blockTextures;
layout(binding = 1) uniform sampler2DArray shadowMap;
layout(binding = 2) uniform sampler2D opaqueDepthMap;

// clang-format off
{{CSM_functions.glsl}}
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

void main() {
    vec4 col = texture(blockTextures, vec3(uv, texID));

    float shadow = calculateShadow(fragPos);
    vec3 lighting = phongLighting(col, shadow);

    vec4 finalColor = vec4(lighting, col.a);
    float weight = clamp(
        pow(min(1.0, finalColor.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 4.5),
        1e-2,
        3e3
    );

    // store pixel color accumulation
    accum = vec4(finalColor.rgb * finalColor.a, finalColor.a) * weight;

    // store pixel revealage threshold
    reveal = finalColor.a;
}