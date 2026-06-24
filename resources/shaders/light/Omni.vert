
#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
layout(location = 3) in uint aData;


layout(std430, binding = 0) readonly buffer ModelMatrices {
    mat4 models[];
};

void main() {
    gl_Position = models[gl_DrawID] * vec4(aPos, 1.0);
}