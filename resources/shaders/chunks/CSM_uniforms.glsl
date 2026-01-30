layout(std140) uniform LightSpaceMatrices {
    mat4 lightSpaceMatrices[{{CascadeCount}}];
};
const int cascadeCount = {{CascadeCount}};
uniform float cascadePlaneDistances[cascadeCount];

uniform vec3 lightColor;
uniform vec3 lightDir;  // negated before passing to shader (pointing towards light)
uniform vec3 viewPos;
uniform vec2 resolution;

uniform mat4 view;