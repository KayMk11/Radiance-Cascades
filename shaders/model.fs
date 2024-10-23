#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

struct Material
{
    sampler2D diffuse;
    sampler2D specular;

    float shininess;
};

uniform Material material;

void main()
{
    FragColor = texture(material.specular, TexCoord);
}