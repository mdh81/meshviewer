#version 410 core
in vec3 colorVS;
out vec4 colorFS;
void main()
{
    colorFS = vec4(colorVS.r, colorVS.g, colorVS.b, 1.0);
}
