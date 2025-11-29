#version 330 core

in vec4 v_position;
out vec4 FragColor;

float C = 45.0;

vec2 VSM(float depth) {
    float moment2 = depth * depth;

    // Partial derivative to reduce shadow acne
    float dx = dFdx(depth);
    float dy = dFdy(depth);
    moment2 += 0.05 * (dx * dx + dy * dy);
    return vec2(depth, moment2);
}

vec4 EVMS(float depth) {
    return vec4(exp(C * depth), exp(C * 2 * depth), exp(-C * depth), exp(-C * 2 * depth));
}


void main() {
    float depth = v_position.z / v_position.w;
    depth = depth * 0.5 + 0.5;

    FragColor = EVMS(depth);
}