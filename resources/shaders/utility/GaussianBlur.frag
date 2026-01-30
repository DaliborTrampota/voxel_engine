#version 330 core

in vec2 uv;
out vec4 FragColor;

uniform sampler2D inputTex;
uniform float texelSize;  // 1.0 / textureWidth
uniform bool horizontal;
const int taps = 3;

// 5-tap Gaussian weights
const float five[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
// 3-tap Gaussian weights (sharper, tighter blur)
const float three[3] = float[](0.4026, 0.2442, 0.0545);

vec4 blur(bool horizontal, float[taps] weights) {
    vec4 result = texture(inputTex, uv) * weights[0];
    if (horizontal) {
        for (int i = 1; i < taps; i++) {
            result += texture(inputTex, uv + vec2(texelSize * i, 0.0)) * weights[i];
            result += texture(inputTex, uv - vec2(texelSize * i, 0.0)) * weights[i];
        }
    } else {
        for (int i = 1; i < taps; i++) {
            result += texture(inputTex, uv + vec2(0.0, texelSize * i)) * weights[i];
            result += texture(inputTex, uv - vec2(0.0, texelSize * i)) * weights[i];
        }
    }
    return result;
}

void main() {
    vec4 result = blur(horizontal, three);
    FragColor = result;
}
