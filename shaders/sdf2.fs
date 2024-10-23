#version 330 core
out vec4 FragColor;

in vec2 UV;
uniform vec2 center;
uniform float radius;
uniform vec3 color;

float sdfCircle(vec2 p, vec2 center, float radius) {
    return length(p - center) - radius;
}

void main()
{
    float dist = sdfCircle(UV, center, radius);
    vec4 background = vec4(0,0,0,1);

    if (dist < 0) {
        FragColor = vec4(color, 1.0);
    } else {
        FragColor = background;  // Transparent or background color
    }
    // FragColor = vec4(1.0,0,0,0);
}