#version 410 core
in float colorVS;
out vec4 colorFS;
void main()
{
    colorFS = vec4(colorVS, colorVS, colorVS, 1.0);
}
