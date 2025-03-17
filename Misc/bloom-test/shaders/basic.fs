#version 450 core
#define MAX_LIGHTS 10

// Structure for a single light.
struct LightData {
    vec4 position;   // xyz: world-space position
    vec4 color;      // rgb: light color, w: intensity
};

// Uniform block for lights. Using std140 layout requires padding,
// so we declare the number of lights as an ivec4 (using only x).
layout(std140, binding = 0) uniform LightBlock {
    ivec4 numLights; // Only numLights.x is used.
    LightData lights[MAX_LIGHTS];
};

uniform vec3 ambientColor; // Global ambient light
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;

//
// Material structure that must match BasicMaterial
//
struct Material {
    vec3 diffuseColor;
    vec3 specularColor;
    float specularPower;
    
    int emissiveEnabled;
    vec3 emissiveColor;
};
uniform Material material;

uniform sampler2D texSampler;

in vec3 FragPos;
flat in vec3 Normal; // Use flat qualifier to avoid interpolation artifacts on sharp edges.
in vec2 TexCoords;

out vec4 FragColor;

void main() {
    // Normalize the provided normal.
    vec3 norm = normalize(Normal);
    // Compute the view direction.
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Ambient component.
    vec3 ambient = ambientColor * material.diffuseColor;
    
    vec3 diffuseSum = vec3(0.0);
    vec3 specularSum = vec3(0.0);
    
    // Loop over lights using the count stored in numLights.x.
    for (int i = 0; i < numLights.x; i++) {
        vec3 lightPos = lights[i].position.xyz;
        vec3 lightColor = lights[i].color.rgb;
        float intensity = lights[i].color.w;
        
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor * material.diffuseColor;
        
        // Calculate the reflection vector and specular term.
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = 0.0;
        if(diff > 0.0)
            spec = pow(max(dot(viewDir, reflectDir), 0.0), material.specularPower);
        vec3 specular = spec * lightColor * material.specularColor;
        
        // Attenuation (based on distance)
        float distance = length(lightPos - FragPos);
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));
        
        diffuseSum += diffuse * intensity * attenuation;
        specularSum += specular * intensity * attenuation;
    }
    
    // Sample the texture.
    vec3 texColor = texture(texSampler, TexCoords).rgb;
    // Combine ambient, diffuse, and specular contributions.
    vec3 finalColor = (ambient + diffuseSum + specularSum) * texColor;
    
    // Add emissive term if enabled.
    if (material.emissiveEnabled == 1)
        finalColor += material.emissiveColor;
    
    FragColor = vec4(finalColor, 1.0);
}
