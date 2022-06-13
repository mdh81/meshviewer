#include "Camera.h"
#include "Mesh.h"
#include "Types.h"


namespace meshviewer {

using namespace common;

Camera::Camera(const Mesh& mesh, const GLuint shaderProgram, const ProjectionType projectionType) 
    : m_mesh(mesh)
    , m_shaderProgram(shaderProgram)
    , m_projectionType(projectionType) {
    
}

void Camera::apply() {
    // Get the composite transform matrix in the shader
    GLuint matrixId = glGetUniformLocation(m_shaderProgram, "transformMatrix");

    // Assemble model, view and projection matrices
    glm::mat4 modelMatrix;
    getModelTransform(modelMatrix);

    glm::mat4 viewMatrix;
    getViewTransform(viewMatrix);

    glm::mat4 projectionMatrix;
    getProjectionTransform(projectionMatrix); 

    // Combined Transform
    // The matrix multiplication order is the reverse order of actual transformations
    // Object -> Global, Global -> Camera, Camera -> Homogenous Coordinates 
    // TODO: Setup an octave session and observe the conversion to homogenous coordinates
    glm::mat4 compositeTransform = projectionMatrix * viewMatrix * modelMatrix;

    glUniformMatrix4fv(matrixId,
                       1 /*num matrices*/,
                       GL_FALSE /*transpose*/,
                       &compositeTransform[0][0]);
}

void Camera::getModelTransform(glm::mat4& modelMatrix) {
    // Convert the mesh vertices from object to world coordinates. 
    // Input mesh's coordinates are in global system, so use an identity matrix 
    modelMatrix = glm::mat4(1.0);
}

void Camera::getViewTransform(glm::mat4& viewMatrix) {
    // Converts the mesh vertices from world coordinate system to 
    // camera coordinate system. Camera in OpenGL is fixed at global origin looking
    // down the -Z axis. Moving the model so it is within the frame of the camera
    // is accomplished by the view transform
    viewMatrix = glm::mat4(1.0f);
    viewMatrix[3] = glm::vec4(0,0,-20,1);
}

void Camera::getProjectionTransform(glm::mat4& projectionMatrix) {
    projectionMatrix = glm::perspective(glm::radians(45.f), 640.f/480.f, 0.1f, 100.0f);
}

}

