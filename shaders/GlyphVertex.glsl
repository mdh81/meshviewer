#version 410 core

in vec3 vertexWorld;
uniform vec3 lineColor;
uniform mat4 modelViewTransform;
uniform mat4 projectionTransform;
out vec3 vertexColor;
out vec3 vertexCamera;

void main() {
    vertexCamera = (modelViewTransform * vec4(vertexWorld, 1.0)).xyz;
    vertexColor = lineColor;
    gl_Position = projectionTransform * modelViewTransform * vec4(vertexWorld, 1.0);
}
