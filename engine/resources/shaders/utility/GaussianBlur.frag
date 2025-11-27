#version 330 core

in vec2 uv;
out vec2 FragColor;

uniform sampler2D inputTex;
uniform float texelSize; // 1.0 / textureWidth
uniform bool horizontal;

// 5-tap Gaussian weights
const float weights[5] = float[](
    0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216
);

void main() {
    if (horizontal) {
        vec2 result = texture(inputTex, uv).rg * weights[0];
        
        for(int i = 1; i < 5; i++) {
            result += texture(inputTex, uv + vec2(texelSize * i, 0.0)).rg * weights[i];
            result += texture(inputTex, uv - vec2(texelSize * i, 0.0)).rg * weights[i];
        }
        FragColor = result;
    } else {
        vec2 result = texture(inputTex, uv).rg * weights[0];
        
        for(int i = 1; i < 5; i++) {
            result += texture(inputTex, uv + vec2(0.0, texelSize * i)).rg * weights[i];
            result += texture(inputTex, uv - vec2(0.0, texelSize * i)).rg * weights[i];
        }   
        FragColor = result;
    }
}