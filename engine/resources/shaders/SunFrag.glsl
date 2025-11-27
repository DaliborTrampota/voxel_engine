#version 330 core

in vec4 v_position;

out vec2 FragColor;

void main()
{
    // VSM: Store depth and depth squared
    float depth = v_position.z / v_position.w;
    depth = depth * 0.5 + 0.5; // Convert to [0,1] range
    
    float moment1 = depth;
    float moment2 = depth * depth;
    
    // Partial derivative to reduce shadow acne
    float dx = dFdx(depth);
    float dy = dFdy(depth);
    moment2 += 0.05 * (dx * dx + dy * dy);
    
    FragColor = vec2(moment1, moment2);
}