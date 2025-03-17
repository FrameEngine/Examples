#version 450 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D sceneTexture;
uniform float threshold;      
uniform float softThreshold; 

void main()
{
    vec3 color = texture(sceneTexture, TexCoords).rgb;

    // Compute perceived brightness using standard luminance coefficients.
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));

    // Use smoothstep to produce a smooth transition:
    // If brightness is below (threshold - softThreshold) => factor = 0.
    // If brightness is above (threshold + softThreshold) => factor = 1.
    float factor = smoothstep(threshold - softThreshold, threshold + softThreshold, brightness);
    
    FragColor = vec4(color * factor, 1.0);
}
