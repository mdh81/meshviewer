#version 410 core

in vec3 vertexCoordinatesIn;
in vec2 textureCoordinatesIn;
out vec2 textureCoordinates;
uniform mat4 orthographicProjectionMatrix;
uniform mat4 axisTransformMatrix;

void main() {
    gl_Position = orthographicProjectionMatrix * axisTransformMatrix * vec4(vertexCoordinatesIn, 1.0);
    textureCoordinates = textureCoordinatesIn;
}