#version 330 core
out vec4 FragColor;

in vec2 UV;
uniform sampler2D tex;
uniform vec2 linestart;
uniform vec2 lineend;
uniform vec3 color;

float sdfLine(vec2 p, vec2 a, vec2 b) {
    vec2 pa = p - a;
    vec2 ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
    return length(pa - ba * h);
}

void main()
{
    float dist = sdfLine(UV,linestart/800, lineend/800);
    vec4 background = texture(tex, UV);

    float lineThickness = 0.01;

    // Hard cutoff for the line
    if (dist < lineThickness) {
        FragColor = vec4(color, 1.0);  // White line
    } else {
        FragColor = background;  // Transparent or background color
    }
    // FragColor = vec4(1.0,0,0,0);
}