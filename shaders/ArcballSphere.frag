#version 410 core

precision highp float;
uniform vec4 sphereColor;
out vec4 fragmentColor;

void main() {
    fragmentColor = vec4(0.5, 0.5, 0.5, 0.2);
}