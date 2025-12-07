#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
layout(location = 3) in uint aData;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec4 vertPosition;
out vec2 uv;
flat out uint texID;

void main() {
    texID = aData & 1023u;
    uv = aUV;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vertPosition = gl_Position;
}