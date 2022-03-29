#version 410 core
in vec3 colorVS;
out vec4 colorFS;
void main()
{
    colorFS = vec4(colorVS, 1.0);
}
