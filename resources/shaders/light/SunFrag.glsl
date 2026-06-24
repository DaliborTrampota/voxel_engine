#version 460 core

layout(std140) uniform LightSpaceMatrices {
    mat4 lightSpaceMatrices[{{CascadeCount}}];
};

uniform sampler2DArray blockTextures;  // unit 0

in Data {
    vec4 vertPosition;
    vec2 uv;
    flat uint texID;
}
fs_in;

out vec4 FragColor;


const float C = 45.0;
const float ALPHA_THRESHOLD = 0.5;

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


// void main() {
//     vec4 texColor = texture(blockTextures, vec3(uv, texID));
//     if (texColor.a < ALPHA_THRESHOLD) {
//         discard;  // Don't cast shadow for transparent pixels
//     }

//     float depth = vertPosition.z / vertPosition.w;
//     depth = depth * 0.5 + 0.5;

//     FragColor = vec4(VSM(depth), 0, 0);
// }

void main() {
    vec4 texColor = texture(blockTextures, vec3(fs_in.uv, fs_in.texID));
    if (texColor.a < ALPHA_THRESHOLD) {
        discard;  // Don't cast shadow for transparent pixels
    }
}