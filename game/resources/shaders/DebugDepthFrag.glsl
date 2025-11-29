#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D depthMap;
uniform float nearPlane;
uniform float farPlane;

// EVSM constant - MUST match your ChunkFrag.glsl and SunFrag.glsl
const float C = 60.0;

void main() {
    vec4 evsmData = texture(depthMap, TexCoords);

    // EVSM stores: (exp(C*d), exp(2C*d), exp(-C*d), exp(-2C*d))

    // Check if we have valid data
    if (evsmData.x < 0.0001 && evsmData.z < 0.0001) {
        // No data - show magenta for debugging
        FragColor = vec4(1.0, 0.0, 1.0, 1.0);
        return;
    }

    // Convert positive warp back to depth: d = ln(exp(C*d)) / C
    float depth = 0.0;
    if (evsmData.x > 0.0001) {
        depth = log(evsmData.x) / C;
    }

    // Clamp to valid range
    depth = clamp(depth, 0.0, 1.0);

    // Visualize: darker = closer, lighter = further
    // FragColor = vec4(vec3(depth), 1.0);

    // DEBUG OPTIONS (uncomment one to try):

    // Show raw exponential values (scaled down massively)
    // FragColor = vec4(evsmData.x * 0.00001, evsmData.y * 0.000001, evsmData.z * 0.1, 1.0);

    // Show if data exists (green = has data, red = no data)
    // FragColor = evsmData.x > 0.0001 ? vec4(0.0, 1.0, 0.0, 1.0) : vec4(1.0, 0.0, 0.0, 1.0);

    // Show all 4 channels as color (will look weird but useful for debugging)
    FragColor =
        vec4(log(evsmData.x) / C, log(evsmData.y) / (2.0 * C), 1.0 - log(evsmData.z) / -C, 1.0);
}
