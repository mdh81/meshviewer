#version 410 core

in vec3 vertexWorld;
in vec3 vertexNormal;
uniform mat4 projectionTransform;
uniform mat4 modelViewTransform;
uniform vec3 reflectionCoefficient;
uniform vec3 lightPosition;
uniform vec3 lightIntensity; 
out vec3 vertexColor;
out vec3 vertexCamera;

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

    // NOTE: To support two-sided lighting, we flip the normal if we
    // encounter back faces
    
    mat3 normalMatrix = mat3(modelViewTransform);
    vec3 normalDirection = normalMatrix * vertexNormal;
    vec3 vertexLocation = convertToViewCoordinates(vertexWorld);
    vec3 lightDirection = normalize(vec3(lightPosition - vertexLocation));
    vec3 vertexToCamera = -vertexLocation;
    vec3 diffuseColor;
    if (dot(vertexToCamera, normalDirection) < 0) {
        diffuseColor = reflectionCoefficient * lightIntensity * max (dot(-normalDirection, lightDirection), 0);
    } else {
        diffuseColor = reflectionCoefficient * lightIntensity * max (dot(normalDirection, lightDirection), 0);
    }
    return diffuseColor;
}

void main() {
    vertexCamera = convertToViewCoordinates(vertexWorld);
    vertexColor = diffuseShading();
    gl_Position = projectionTransform * modelViewTransform * vec4(vertexWorld, 1.0);
}
