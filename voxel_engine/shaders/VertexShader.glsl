#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 col;
out vec3 pos;

void main()
{
	col = aCol;
    pos = aPos;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}