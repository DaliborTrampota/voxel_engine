
#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
layout(location = 3) in uint aData;


layout(std430, binding = 0) readonly buffer ModelMatrices {
    mat4 models[];
};

uniform mat4 projection;
uniform mat4 view;

out vec3 fragPos;

void main() {
    fragPos = vec3(models[gl_DrawID] * vec4(aPos, 1.0));
    gl_Position = projection * view * models[gl_DrawID] * vec4(aPos, 1.0);
}