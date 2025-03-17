#version 450 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D image;
uniform float texelHeight; // 1.0 / texture height

void main()
{
    float centerWeight = 0.4;
    float sideWeight = 0.3;
    float offset = texelHeight * 4.0;
    
    vec3 result = texture(image, TexCoords).rgb * centerWeight;
    result += texture(image, TexCoords + vec2(0.0, offset)).rgb * sideWeight;
    result += texture(image, TexCoords - vec2(0.0, offset)).rgb * sideWeight;
    
    FragColor = vec4(result, 1.0);
}
