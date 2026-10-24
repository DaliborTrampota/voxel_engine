#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
layout(location = 3) in uint aData;

// clang-format off
{{SeaSurface_functions.glsl}}
// clang-format on


layout(std430, binding = 0) readonly buffer ModelMatrices {
    mat4 models[];
};


out Data {
    vec4 vertPosition;
    vec2 uv;
    flat uint texID;
}
vs_out;

void main() {
    // clang-format off
    vs_out.texID = aData & 65535u;
    // clang-format on
    vs_out.uv = aUV;

    // for VSM
    // gl_Position = projection * view * model * vec4(aPos, 1.0);
    // vs_out.vertPosition = gl_Position;
    vec4 worldPos = models[gl_DrawID] * vec4(aPos, 1.0);
    if ((aData & SEA_SURFACE_BIT) != 0u) {
        vec3 normal = mat3(models[gl_DrawID]) * aNormal;
        worldPos.y = seaSurfaceY(worldPos.y, seaSurfaceWaves(worldPos.xz, worldPos.y, aData), aData, abs(normal.y) < 0.5);
    }
    gl_Position = worldPos;
}