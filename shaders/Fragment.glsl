#version 410 core
precision highp float;
in vec3 vertexCamera;
uniform struct {
    float minimumDistance;
    float maximumDistance;
    vec3 color;
    bool enabled;
} fog;
in vec3 vertexColor;
out vec4 fragmentColor;
void main() {
    if (fog.enabled) {
        float distanceFromCamera = length(vertexCamera);
        float fogFactor = (distanceFromCamera - fog.minimumDistance) / (fog.maximumDistance - fog.minimumDistance);
        fragmentColor = vec4(mix(vertexColor, fog.color, fogFactor), 1.0);
    } else {
        fragmentColor = vec4(vertexColor.r, vertexColor.g, vertexColor.b, 1.0);
    }
}
