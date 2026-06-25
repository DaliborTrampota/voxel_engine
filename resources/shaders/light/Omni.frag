#version 460 core

uniform vec3 lightPos;
uniform float farPlane;

in vec3 fragPos;

// TODO alpha discard
void main() {
    gl_FragDepth = length(fragPos - lightPos) / farPlane;
}