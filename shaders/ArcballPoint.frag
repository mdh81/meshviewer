#version 410 core

precision highp float;
uniform vec4 pointColor;
in vec2 textureCoordinates;
out vec4 fragmentColor;
uniform sampler2D textureSampler;

void main() {
    fragmentColor = pointColor * texture(textureSampler, textureCoordinates);
}