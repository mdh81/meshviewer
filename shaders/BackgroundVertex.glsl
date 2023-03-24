#version 410 core

in vec3 vertexPosition;
in vec3 vertexColorIn;
out vec3 vertexColor;

// A simple pass through shader for rendering the gradient background
// The vertex coordinates are assumed to be in normalized device coordinate space

void main() {
    vertexColor = vertexColorIn;
    gl_Position = vec4(vertexPosition, 1.0);
}