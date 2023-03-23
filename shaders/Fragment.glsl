#version 410 core
in vec3 vertexColor;
out vec4 fragmentColor;
void main() {
    fragmentColor = vec4(vertexColor.r, vertexColor.g, vertexColor.b, 1.0);
}
