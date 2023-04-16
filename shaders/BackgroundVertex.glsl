#version 410 core

in vec3 vertexPosition;
in vec3 vertexColorIn;
out vec3 vertexColor;
uniform mat4 orthographicProjectionMatrix;

// The vertex coordinates are assumed to be in camera coordinate space and
// are converted to NDC using the provided orthographic projection matrix

void main() {
    vertexColor = vertexColorIn;
    gl_Position = orthographicProjectionMatrix * vec4(vertexPosition, 1.0);
}