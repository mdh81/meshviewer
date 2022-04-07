#version 410 core

in vec2 position;
in float color;
out float colorVS;

void main()
{
    colorVS = color;
    gl_Position = vec4(position, 0.0, 1.0);
}
