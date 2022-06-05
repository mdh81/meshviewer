#version 410 core

in vec3 position;
in vec3 wireframeColor;
out vec3 colorVS;
uniform mat4 transformMatrix;

void main()
{
    colorVS = wireframeColor;
    gl_Position = transformMatrix * vec4(position, 1.0);
}
