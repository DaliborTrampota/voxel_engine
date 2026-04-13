#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
layout(location = 3) in uint aData;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 pos;
out vec3 normal;
out vec2 uv;
out vec3 fragPos;

// out ShadowData {
//     // vec4 fragPosLightSpace;
// }
// shadowData;


flat out uint texID;
out float ao;

void main() {
    texID = aData & 65535u;
    ao = 1.0 - ((aData >> 16) & 3u) / 5.0;

    uv = aUV;
    pos = aPos;

    //mat3 normalMatrix = transpose(inverse(mat3(model))); // not needed when we dont do non-uniform scaling
    normal = mat3(model) * aNormal;

    fragPos = vec3(model * vec4(aPos, 1.0));
    // shadowData.fragPos = vec3(model * vec4(aPos, 1.0));
    // shadowData.fragPosLightSpace = lightSpaceTransform * vec4(shadowData.fragPos, 1.0);

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}