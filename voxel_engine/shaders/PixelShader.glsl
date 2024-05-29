#version 330 core

out vec4 FragColor;
in vec3 col;
in vec3 pos;

uniform float time;


void main()
{
    FragColor = vec4(pos, 1.0);
    //FragColor.g = mod(time, 1);
}