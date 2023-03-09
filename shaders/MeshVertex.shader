#version 410 core

in vec3 vertexPosition;
in vec3 vertexNormal;
uniform mat4 modelViewProjectionTransform;
uniform mat4 modelViewTransform;
uniform vec3 reflectionCoefficient;
uniform vec3 lightPosition;
uniform vec3 lightIntensity; 
out vec3 vertexColor;

// Converts a vector from world coordinates to view coordinates
vec3 convertToViewCoordinates(vec3 vectorWorld) {
    return (modelViewTransform * vec4(vectorWorld, 1.0)).xyz;
}

vec3 diffuseShading() {
    
    // Compute diffuse light at each vertex by evaluating the
    // diffuse light as a RGB vector that is scaled by the dot
    // product of the light and normal directions. This way, the 
    // intensity of reflected light at a vertex becomes dependent
    // on the vertex's orientation relative to the light 
    
    mat3 normalMatrix = mat3(modelViewTransform);
    vec3 normalDirection = normalMatrix * vertexNormal;
    vec3 vertexLocation = convertToViewCoordinates(vertexPosition);
    vec3 lightDirection = normalize(vec3(lightPosition - vertexLocation));
    return reflectionCoefficient * lightIntensity * max (dot(normalDirection, lightDirection), 0);
}

void main() {
    vertexColor = diffuseShading();
    gl_Position = modelViewProjectionTransform * vec4(vertexPosition, 1.0);
}
