#version 330 core

out vec4 FragColor;
in vec3 col;
in vec3 pos;
in vec3 normal;

uniform float time;

vec3 sunDir = normalize(vec3(0.2, 1, 0.2));

void main()
{
    vec3 lightCol = col * max(0.2, dot(normal, sunDir));
	FragColor = vec4(lightCol, 1.0);
    //FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    //FragColor.g = mod(time, 1);
}