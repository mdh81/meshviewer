#version 410 core

in vec3 vertexPosition;
in vec3 lineColor; 
uniform mat4 modelViewProjectionTransform;
out vec3 vertexColor;

void main() {
    vertexColor = lineColor;
    gl_Position = modelViewProjectionTransform * vec4(vertexPosition, 1.0);
}
