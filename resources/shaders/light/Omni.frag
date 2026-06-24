#version 460 core

uniform vec3 lightPos;
uniform float farPlane;

in vec3 fragPos;
in vec3 pos;
in vec2 uv;
flat in uint texID;

void main() {
    gl_FragDepth = length(fragPos - lightPos) / farPlane;
}