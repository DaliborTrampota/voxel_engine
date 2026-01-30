#version 460 core

layout(location = 0) out vec4 frag;

layout(binding = 0) uniform sampler2D accum;
layout(binding = 1) uniform sampler2D reveal;
layout(binding = 2) uniform sampler2D opaque;

const float EPSILON = 0.00001;

// calculate floating point numbers equality accurately
bool isApproximatelyEqual(float a, float b) {
    return abs(a - b) <= (abs(a) < abs(b) ? abs(b) : abs(a)) * EPSILON;
}

// get the max value between three values
float max3(vec3 v) {
    return max(max(v.x, v.y), v.z);
}

void main() {
    ivec2 coords = ivec2(gl_FragCoord.xy);

    vec4 opaqueColor = texelFetch(opaque, coords, 0);
    float revealage = texelFetch(reveal, coords, 0).r;

    if (isApproximatelyEqual(revealage, 1.0)) {
        frag = opaqueColor;
        return;
    }

    vec4 accumulation = texelFetch(accum, coords, 0);

    // suppress overflow
    if (isinf(max3(abs(accumulation.rgb))))
        accumulation.rgb = vec3(accumulation.a);

    // prevent floating point precision bug
    vec3 average_color = accumulation.rgb / max(accumulation.a, EPSILON);

    // blend pixels
    frag = vec4(mix(average_color, opaqueColor.rgb, revealage), 1.0);
}