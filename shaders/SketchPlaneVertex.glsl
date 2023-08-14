#version 410 core

in vec3 sketchPlaneVertex;
uniform mat4 projectionTransform;
out vec4 vertexNDC;

void main() {
    vertexNDC = projectionTransform * sketchPlaneVertex;
    gl_position = vertexNDC;
}