#version 450 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D image;
uniform float texelWidth; // 1.0 / texture width

void main()
{
    float centerWeight = 0.4;
    float sideWeight = 0.3;
    // Increase offset multiplier for a wider blur.
    float offset = texelWidth * 4.0;
    
    vec3 result = texture(image, TexCoords).rgb * centerWeight;
    result += texture(image, TexCoords + vec2(offset, 0.0)).rgb * sideWeight;
    result += texture(image, TexCoords - vec2(offset, 0.0)).rgb * sideWeight;
    
    FragColor = vec4(result, 1.0);
}
