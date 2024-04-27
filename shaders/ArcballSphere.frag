#version 410 core

precision highp float;
uniform vec4 sphereColor;
out vec4 fragmentColor;

void main() {
    fragmentColor = sphereColor;
}