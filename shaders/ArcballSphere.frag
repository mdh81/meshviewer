#version 410 core

uniform vec4 sphereColor;
out vec4 fragmentColor;

void main() {
    fragmentColor = sphereColor;
}