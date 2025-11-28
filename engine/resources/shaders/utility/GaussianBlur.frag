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

vec4 blur(bool horizontal) {
        vec4 result = texture(inputTex, uv) * weights[0];
    if (horizontal) {
        for(int i = 1; i < 5; i++) {
            result += texture(inputTex, uv + vec2(texelSize * i, 0.0)) * weights[i];
            result += texture(inputTex, uv - vec2(texelSize * i, 0.0)) * weights[i];
        }
    } else {
        for(int i = 1; i < 5; i++) {
            result += texture(inputTex, uv + vec2(0.0, texelSize * i)) * weights[i];
            result += texture(inputTex, uv - vec2(0.0, texelSize * i)) * weights[i];
        }
    }
    return result;
}

float depthWeight(float sampleDepth, float centerDepth)
{
    float diff = abs(sampleDepth - centerDepth);
    float sigma = 0.002;        // tunable: smaller = sharper edges
    return exp(-(diff * diff) / (2.0 * sigma * sigma));
}

void main() {
    vec4 result = blur(horizontal);
    FragColor = result.rg;
}
