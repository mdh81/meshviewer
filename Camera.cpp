#include "Camera.h"
#include "Mesh.h"
#include "Types.h"


namespace meshviewer {

using namespace common;

void Camera::apply(const GLuint shaderProgram) {
    // Get the composite transform matrix in the shader
    GLuint matrixId = glGetUniformLocation(shaderProgram, "transformMatrix");

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
    
    // Get the centroid of the mesh 
    // Translate the mesh so it's at world origin
    auto translateToOrigin = glm::mat4(1.0f);
    common::Vertex centroid = m_mesh.getCentroid();
    translateToOrigin[3] = glm::vec4(-centroid.x, -centroid.y, -centroid.z, 1.f);

    common::Bounds bounds = m_mesh.getBounds();

    // Translate along -Z by the length of the mesh along Z axis so the camera
    // is in front of the mesh by a distance of zlen(mesh)/2
    auto moveBack = glm::mat4(1.0f);
    moveBack[3] = glm::vec4(0, 0, -1*bounds.zlen(), 1);

    static bool printed = false;
    if (!printed && m_debugOn) {
        std::cout << "Mesh Centroid: " << centroid << std::endl;
        std::cout << "Mesh Bounds: " << bounds << std::endl;
        std::cout << "Moving the mesh back along Z by " << bounds.zlen() << std::endl;
        printed = true;
    }
    
    // Combine the translations and produce the view transform 
    viewMatrix = moveBack * translateToOrigin;
}

}

