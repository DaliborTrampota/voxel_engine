#version 460 core

layout(triangles, invocations = 6) in;  // 6 faces of the cube
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 faceMatrices[6];
uniform int baseLayer;

out vec3 fragPos;

void main() {
    for (int i = 0; i < 3; ++i) {
        vec4 worldPos = gl_in[i].gl_Position;
        fragPos = worldPos.xyz;
        gl_Layer = baseLayer + gl_InvocationID;
        gl_Position = faceMatrices[gl_InvocationID] * worldPos;
        EmitVertex();
    }
    EndPrimitive();
}