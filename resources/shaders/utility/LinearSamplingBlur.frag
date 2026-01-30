#version 330 core

in vec2 uv;
out vec4 FragColor;

uniform sampler2D inputTex;
uniform float texelSize;  // 1.0 / textureWidth
uniform bool horizontal;

const float offsets[2] = float[](1.3846153846, 3.2307692308);
const float weights[2] = float[](0.3162162162, 0.0702702703);
const float centerWeight = 0.2270270270;

vec4 blur(bool horizontal) {
    vec4 result = texture(inputTex, uv) * centerWeight;
    if (horizontal) {
        for (int i = 0; i < 2; i++) {
            float offset = texelSize * offsets[i];
            result += texture(inputTex, uv + vec2(offset, 0.0)) * weights[i];
            result += texture(inputTex, uv - vec2(offset, 0.0)) * weights[i];
        }
    } else {
        for (int i = 0; i < 2; i++) {
            float offset = texelSize * offsets[i];
            result += texture(inputTex, uv + vec2(0.0, offset)) * weights[i];
            result += texture(inputTex, uv - vec2(0.0, offset)) * weights[i];
        }
    }
    return result;
}

void main() {
    vec4 result = blur(horizontal);
    FragColor = result;
}
