#version 410 core

precision highp float;
in vec3 vertexCoordinatesIn;
in vec2 textureCoordinatesIn;
out vec2 textureCoordinates;
uniform mat4 orthographicProjectionMatrix;
uniform mat4 pointTransformMatrix;

void main() {
    gl_Position = orthographicProjectionMatrix * pointTransformMatrix * vec4(vertexCoordinatesIn, 1.0);
    textureCoordinates = textureCoordinatesIn;
}