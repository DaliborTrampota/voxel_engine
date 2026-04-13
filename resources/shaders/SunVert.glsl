#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
layout(location = 3) in uint aData;

// uniform mat4 projection;
// uniform mat4 view;
uniform mat4 model;

out Data {
    vec4 vertPosition;
    vec2 uv;
    flat uint texID;
}
vs_out;

void main() {
    vs_out.texID = aData & 65535u;
    vs_out.uv = aUV;

    // gl_Position = projection * view * model * vec4(aPos, 1.0);
    // vs_out.vertPosition = gl_Position;
    gl_Position = model * vec4(aPos, 1.0);
}