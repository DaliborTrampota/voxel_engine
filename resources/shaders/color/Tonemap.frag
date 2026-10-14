#version 460 core

layout(location = 0) out vec4 frag;
in vec2 uv;

layout(binding = 0) uniform sampler2D hdrColor;
uniform float exposure;
uniform int tonemapper;  // 0 = Khronos PBR Neutral, 1 = AgX

vec3 pbrNeutral(vec3 color) {
    const float startCompression = 0.8 - 0.04;
    const float desaturation = 0.15;
    float x = min(color.r, min(color.g, color.b));
    float offset = x < 0.08 ? x - 6.25 * x * x : 0.04;
    color -= offset;
    float peak = max(color.r, max(color.g, color.b));
    if (peak < startCompression)
        return color;
    const float d = 1.0 - startCompression;
    float newPeak = 1.0 - d * d / (peak + d - startCompression);
    color *= newPeak / peak;
    float g = 1.0 - 1.0 / (desaturation * (peak - newPeak) + 1.0);
    return mix(color, vec3(newPeak), g);
}

vec3 agxContrast(vec3 x) {
    vec3 x2 = x * x;
    vec3 x4 = x2 * x2;
    return 15.5 * x4 * x2 - 40.14 * x4 * x + 31.96 * x4 - 6.868 * x2 * x + 0.4298 * x2 +
           0.1191 * x - 0.00232;
}

// Output is already display-encoded; do not apply linearToSRGB after it.
vec3 agx(vec3 color) {
    const mat3 inset = mat3(
        0.842479062253094, 0.0423282422610123, 0.0423756549057051,
        0.0784335999999992, 0.878468636469772, 0.0784336,
        0.0792237451477643, 0.0791661274605434, 0.879142973793104
    );
    const mat3 outset = mat3(
        1.19687900512017, -0.0528968517574562, -0.0529716355144438,
        -0.0980208811401368, 1.15190312990417, -0.0980434501171241,
        -0.0990297440797205, -0.0989611768448433, 1.15107367264116
    );
    const float minEv = -12.47393;
    const float maxEv = 4.026069;
    color = clamp(log2(max(inset * color, 1e-10)), minEv, maxEv);
    color = agxContrast((color - minEv) / (maxEv - minEv));
    return outset * color;
}

vec3 linearToSRGB(vec3 c) {
    return mix(12.92 * c, 1.055 * pow(c, vec3(1.0 / 2.4)) - 0.055, step(0.0031308, c));
}

float dither(vec2 p) {
    return fract(52.9829189 * fract(dot(p, vec2(0.06711056, 0.00583715)))) - 0.5;
}

void main() {
    vec3 hdr = texture(hdrColor, uv).rgb * exposure;

    vec3 ldr = tonemapper == 0 ? linearToSRGB(clamp(pbrNeutral(hdr), 0.0, 1.0))
                               : clamp(agx(hdr), 0.0, 1.0);

    frag = vec4(ldr + dither(gl_FragCoord.xy) / 255.0, 1.0);
}
