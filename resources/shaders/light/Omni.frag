#version 460 core

const float ALPHA_THRESHOLD = 0.5;

uniform sampler2DArray blockTextures;
uniform vec3 lightPos;
uniform float farPlane;

in vec3 fragPos;
in vec2 uv;
in flat uint texID;

void main() {
    vec4 texColor = texture(blockTextures, vec3(uv, texID));
    if (texColor.a < ALPHA_THRESHOLD) {
        discard;
    }
    gl_FragDepth = length(fragPos - lightPos) / farPlane;
}