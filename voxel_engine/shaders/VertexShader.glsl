#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 pos;
out vec3 normal;
out vec2 uv;

flat out uint texID;

void main()
{
    texID = 1u;

    uv = aUV;
    pos = aPos;
    normal = aNormal;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}