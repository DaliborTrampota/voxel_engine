#version 330 core

out vec4 FragColor;
in vec3 pos;
in vec3 normal;
in vec2 uv;

in ShadowData {
    vec3 fragPos;
    vec4 fragPosLightSpace;
} shadowData;

flat in uint texID;
in float ao;

uniform sampler2DArray texArray;
uniform float time;

//TODO UBO?
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 lightDir;
uniform vec3 viewPos;
uniform sampler2D shadowMap;

bool ENABLE_VSM = true;

float linstep(float low, float high, float v) {
    return clamp((v - low) / (high - low), 0.0, 1.0);
}

float VSM(vec3 projCoords, float currentDepth) {
    // Read the moments from the VSM shadow map
    vec2 moments = texture(shadowMap, projCoords.xy).rg;
    
    // Surface is fully lit if the current fragment is before the light occluder
    if (currentDepth <= moments.x)
        return 0.0;
    
    // Calculate variance
    float variance = moments.y - (moments.x * moments.x);
    variance = max(variance, 0.00002); // Minimum variance to avoid division by zero
    
    // Calculate probabilistic upper bound using Chebyshev's inequality
    float d = currentDepth - moments.x;
    float p_max = variance / (variance + d * d);
    
    // Reduce light bleeding (lower = softer, more bleeding)
    float lightBleedReduction = 0.2;
    p_max = linstep(lightBleedReduction, 1.0, p_max);
    
    return 1.0 - p_max;
}




float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    if(projCoords.z > 1.0)
        return 0.0;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow


    if (ENABLE_VSM) {
        return VSM(projCoords, currentDepth);
    } else {
        float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);  
        return currentDepth - bias > closestDepth ? 1.0 : 0.0;
    }
}  


void main()
{
	vec4 col = texture(texArray, vec3(uv, texID));
    // vec3 normal = normalize(normal);
    if (texID == 7u) { // TODO grass coloring
        col.rgb *= vec3(0.4, 0.9, 0.3);
    }
    vec3 ambientCol = 0.3 * lightColor;  // Increased from 0.15 for less dark shadows
    //diffuse
    vec3 lightDir = normalize(lightPos - shadowData.fragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    //specular
    // vec3 viewDir = normalize(viewPos - shadowData.fragPos.xyz);
    // float spec = 0.0;
    // vec3 halfwayDir = normalize(lightDir + viewDir);  
    // spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = vec3(0.0);// spec * lightColor;  


    float shadow = ShadowCalculation(shadowData.fragPosLightSpace, lightDir);
    vec3 lighting = (ambientCol + (1.0 - shadow) * (diffuse + specular)) * col.rgb;

    // vec3 colData = col.xyz * max(0.5, dot(normal, sunDir));
    //FragColor = vec4(uv.x, 0, uv.y, 1.0);//col;
    //FragColor = vec4(lightColor, 1.0);
    FragColor = vec4(lighting, col.a);
}