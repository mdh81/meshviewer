#version 410 core

in vec3 vertexCameraIn;
in vec3 vertexColorIn;
out vec3 vertexColor;
out vec3 vertexCamera;
uniform mat4 orthographicProjectionMatrix;

// The vertex coordinates are assumed to be in camera coordinate space and
// are converted to NDC using the provided orthographic projection matrix

void main() {
    vertexCamera = vertexCameraIn;
    vertexColor = vertexColorIn;
    gl_Position = orthographicProjectionMatrix * vec4(vertexCameraIn, 1.0);
}