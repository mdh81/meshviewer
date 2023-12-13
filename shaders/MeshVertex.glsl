#version 410 core

precision highp float;

in vec3 vertexModel;
in vec3 vertexNormalModel;
uniform mat4 projectionTransform;
uniform mat4 modelViewTransform;
out vec3 vertexColor;
out vec3 vertexCamera;

uniform struct {
    vec3 ambientColor;
    vec3 diffuseColor;
    vec3 specularColor;
    float shininess;
} material;

// NOTE: Light position is assumed to be in view coordinates
// TODO: Support light position in world coordinates
uniform struct {
    vec3 position;
    vec3 color;
} light;

const float specularReflectivity = 0.8;

// Converts a position vector from model to view coordinates
vec3 convertPositionVectorToView(vec3 vectorModel) {
    return (modelViewTransform * vec4(vectorModel, 1.0)).xyz;
}

// Converts a direction vector from model to view coordinates
vec3 convertDirectionVectorToView(vec3 vectorModel) {
    return mat3(modelViewTransform) * vectorModel;
}

vec3 diffuseShading() {

    // Compute diffuse light at each vertex by evaluating the
    // diffuse light as a RGB vector that is scaled by the dot
    // product of the light and normal directions. This way, the
    // intensity of reflected light at a vertex becomes dependent
    // on the vertex's orientation relative to the light

    // NOTE: To support two-sided lighting, we flip the normal if we
    // encounter back faces

    vec3 normalDirection = convertDirectionVectorToView(vertexNormalModel);
    vec3 vertexLocation = convertPositionVectorToView(vertexModel);
    vec3 lightDirection = normalize(vec3(light.position - vertexLocation));
    vec3 vertexToCamera = -vertexLocation;
    vec3 diffuseColor;
    if (dot(vertexToCamera, normalDirection) < 0) {
        diffuseColor = material.diffuseColor * light.color * max(dot(-normalDirection, lightDirection), 0.0);
    } else {
        diffuseColor = material.diffuseColor * light.color * max(dot(normalDirection, lightDirection), 0.0);
    }
    return diffuseColor;
}

vec3 phongShading() {

    // Compute specular light at vertex
    vec3 vertexView = convertPositionVectorToView(vertexModel);
    vec3 vertexNormalCamera = convertDirectionVectorToView(vertexNormalModel);
    vec3 vertexToLight = normalize(light.position - vertexView);
    vec3 parallelToNormal = dot(vertexToLight, vertexNormalCamera) * vertexNormalCamera;
    vec3 perpendicularToNormal = vertexToLight - parallelToNormal;
    vec3 perfectReflection = 2*parallelToNormal - vertexToLight;
    vec3 vertexToCamera = -normalize(vertexView);
    float specularPower = pow(max(dot(perfectReflection, vertexToCamera), 0.0), material.shininess);
    vec3 specularLight = light.color * material.specularColor * specularReflectivity * specularPower;

    // Compute diffuse light
    vec3 diffuseLight = diffuseShading();

    // Ambient light is not directional
    vec3 ambientLight = light.color * material.ambientColor;

    // Combine all components for final result
    return ambientLight + diffuseLight + specularLight;
}

void main() {
    vertexCamera = convertPositionVectorToView(vertexModel);
    vertexColor = phongShading();
    gl_Position = projectionTransform * modelViewTransform * vec4(vertexModel, 1.0);
}
