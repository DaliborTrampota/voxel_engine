#version 330 core

in vec4 vertPosition;
out vec4 FragColor;

const float C = 45.0;

vec2 VSM(float depth) {
    float moment2 = depth * depth;

    // Partial derivative to reduce shadow acne
    float dx = dFdx(depth);
    float dy = dFdy(depth);
    moment2 += 0.005 * (dx * dx + dy * dy);
    return vec2(depth, moment2);
}
//https://martincap.io/projects/evsm/
vec4 EVSM(float depth) {
    return vec4(exp(C * depth), exp(C * 2 * depth), exp(-C * depth), exp(-C * 2 * depth));
}


void main() {
    float depth = vertPosition.z / vertPosition.w;
    depth = depth * 0.5 + 0.5;

    FragColor = vec4(VSM(depth), 0, 0);
}