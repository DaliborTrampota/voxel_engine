#version 460 core

layout(triangles, invocations = 4) in;  // 4 cascades TODO
layout(triangle_strip, max_vertices = 3) out;

layout(std140) uniform LightSpaceMatrices {
    mat4 lightSpaceMatrices[{{CascadeCount}}];
};

in Data {
    vec4 vertPosition;
    vec2 uv;
    flat uint texID;
}
gs_in[];

out Data {
    vec4 vertPosition;
    vec2 uv;
    flat uint texID;
}
gs_out;

void main() {
    for (int i = 0; i < 3; ++i) {
        gl_Position = lightSpaceMatrices[gl_InvocationID] * gl_in[i].gl_Position;
        gl_Layer = gl_InvocationID;

        gs_out.texID = gs_in[i].texID;
        gs_out.uv = gs_in[i].uv;
        gs_out.vertPosition = gs_in[i].vertPosition;

        EmitVertex();
    }
    EndPrimitive();
}