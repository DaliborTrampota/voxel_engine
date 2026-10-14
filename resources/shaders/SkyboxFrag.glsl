#version 460 core

out vec4 FragColor;
in vec3 TexCoords;

uniform samplerCube skybox;
uniform float skyIntensity;
uniform vec3 lightDiskRadiance;

uniform vec3 lightDiskDir;
uniform float lightDiskCosRadius;

void main() {
    float edge = (1.0 - lightDiskCosRadius) * 0.2;
    float disk = smoothstep(lightDiskCosRadius - edge, lightDiskCosRadius + edge, dot(normalize(TexCoords), lightDiskDir));
    FragColor = vec4(texture(skybox, TexCoords).rgb * skyIntensity + disk * lightDiskRadiance, 1.0);

}