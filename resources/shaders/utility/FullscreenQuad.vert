#version 460 core

out vec2 uv;

void main() {
    // Vertex ID: 0, 1, 2
    // Positions: (-1,-1), (3,-1), (-1,3)
    // This oversized triangle covers the entire screen

    vec2 pos = vec2(
                   (gl_VertexID & 1) << 2,  // 0, 4, 0 → -1, 3, -1
                   (gl_VertexID >> 1) << 2  // 0, 0, 4 → -1, -1, 3
               ) -
               1.0;

    uv = (pos + 1.0) * 0.5;  // Convert [-1,1] to [0,1]

    gl_Position = vec4(pos, 0.0, 1.0);
}