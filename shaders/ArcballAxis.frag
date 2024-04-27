#version 410 core

precision highp float;
uniform vec4 axisColor;
in vec2 textureCoordinates;
out vec4 fragmentColor;
uniform sampler2D textureSampler;

void main() {
    fragmentColor = axisColor * texture(textureSampler, textureCoordinates);
}