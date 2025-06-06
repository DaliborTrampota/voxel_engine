#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;
layout (location = 3) in uint aData;

layout (std140) uniform Common
{
    mat4 projection;
    mat4 view;
};
uniform mat4 model;

out vec3 pos;
out vec3 normal;
out vec2 uv;

flat out uint texID;
out float ao;

void main()
{
    texID = aData & 1023u;
    ao = 1.0 -((aData >> 10) & 3u) / 5.0;

    uv = aUV;
    pos = aPos;
    normal = aNormal;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}