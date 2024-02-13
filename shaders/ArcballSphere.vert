#version 410 core

in vec3 sphereVertex;
uniform mat4 orthographicProjectionMatrix;

void main() {
    gl_Position = orthographicProjectionMatrix * vec4(sphereVertex, 1.0);
}