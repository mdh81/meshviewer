#include "Camera.h"
#include "Mesh.h"
#include "Types.h"
#include "EventHandler.h"
#include "CallbackFactory.h"
#include "Util.h"

namespace meshviewer {

using namespace common;
using namespace events;

Camera::Camera(const Mesh& m, const ProjectionType type)
   : m_mesh(m)
   , m_projectionType(type) {

    // Register event handlers for switching between perspective and orthographic 
    EventHandler().registerCallback(Event(GLFW_KEY_O), 
        CallbackFactory::getInstance().registerCallback(
            *this, &Camera::setProjectionType, ProjectionType::Orthographic));

    EventHandler().registerCallback(Event(GLFW_KEY_P), 
        CallbackFactory::getInstance().registerCallback(
            *this, &Camera::setProjectionType, ProjectionType::Perspective));

}

void Camera::apply(const GLuint shaderProgram) {
    // Get the composite transform matrix in the shader
    GLuint matrixId = glGetUniformLocation(shaderProgram, "transformMatrix");

    // Assemble model, view and projection matrices
    buildModelTransform();

    buildViewTransform();

    buildProjectionTransform(); 

    // Combined Transform
    // The matrix multiplication order is the reverse order of actual transformations
    // Object -> Global, Global -> Camera, Camera -> Homogenous Coordinates 
    // TODO: Setup an octave session and observe the conversion to homogenous coordinates
    glm::mat4 compositeTransform = m_projectionTransform * m_viewTransform * m_modelTransform;

    glUniformMatrix4fv(matrixId,
                       1 /*num matrices*/,
                       GL_FALSE /*transpose*/,
                       &compositeTransform[0][0]);
}

void Camera::buildModelTransform() {
    // Convert the mesh vertices from object to world coordinates. 
    // Input mesh's coordinates are in global system, so use an identity matrix 
    m_modelTransform = glm::mat4(1.0);
}

void Camera::buildViewTransform() {
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
    m_viewTransform = moveBack * translateToOrigin;
}

void Camera::buildProjectionTransform() {
    
    if (m_projectionType == ProjectionType::Orthographic) {
        buildOrthographicProjectionTransform();
    } else {
        buildPerspectiveProjectionTransform();
    }

}

void Camera::buildOrthographicProjectionTransform() {
    
    // Create a view volume that is of the same dimensions as
    // the bounding box of the mesh in X and Y directions.
    // Set near plane to zlen/2 and far plane to near + z-len 
    auto bounds = m_mesh.getBounds();
    auto boundsView = Util::transformBounds(bounds, m_viewTransform);

    m_projectionTransform = glm::ortho(1.5f*boundsView.xmin, 1.5f*boundsView.xmax,
                                       1.5f*boundsView.ymin, 1.5f*boundsView.ymax,
                                       boundsView.zlen()*0.5f, boundsView.zlen()*0.5f + boundsView.zlen());
    
    static bool printed = false;
    if (!printed && m_debugOn) {
        std::cout << "Mesh Bounds: " << boundsView << std::endl;
        std::cout << "Left: " << 1.5f*boundsView.xmin << " Right: " << 1.5f*boundsView.xmax
                  << " Bottom: " << 1.5f*boundsView.ymin << " Top: " << 1.5f*boundsView.ymax
                  << " Near: " << boundsView.zlen()*0.5f << " Far: " << boundsView.zlen() + boundsView.zlen() * 0.5f
                  << std::endl;
        printed = true;
    }
}

void Camera::buildPerspectiveProjectionTransform() {
    // Compute the field of view
    
    // The view transform in getViewTransform() has placed the mesh at a distance of
    // of d = zlen(mesh)/2 from the camera, which is always at 0,0,0
    //
    // Side-view of the camera and the mesh             
    //
    // Camera is at the origin A            
    //                                                              +Y
    //                      B--------------------F                  |
    //                      |                    |                  |
    //                      |                    |                  |
    //      A-------d-------C                    |       +Z_________|
    //   (0,0,0)            |                    |
    //                      |                    |
    //                      D--------------------E
    //
    // Fov is the angle between AB and AD
    // BDEF is the bounding box of the mesh
    // 
    // To find this angle, we consider the triangle ABC
    // Angle between AC and BC is 90 degrees. The angle
    // between AC and AB is Fov/2
    //
    // BC = Opposite side = 0.5 * Y-Len of mesh  
    // AC = Adjacent side = 0.5 * z-Len of mesh 
    // tan(fov/2) = BC/AC
    // fov/2 = arctan(BC/AC)
    // fov = 2 * arctan(BC/AC)

    common::Bounds bounds = m_mesh.getBounds();
    auto AC = bounds.zlen() * 0.5f;
    auto BC = bounds.ylen() * 0.5f;
    m_fieldOfView = 2.f * glm::atan(BC/AC);
    m_projectionTransform = glm::perspective(m_fieldOfView, 640.f/480.f, AC, AC + bounds.zlen());

    static bool printed = false;
    if (!printed && m_debugOn) {
        std::cout << "Y-Length = " << bounds.ylen() << std::endl;
        std::cout << "Z-Length = " << bounds.zlen() << std::endl;
        std::cout << "AC = " << AC << std::endl;
        std::cout << "BC = " << BC << std::endl;
        std::cout << "Fov = " << glm::degrees(m_fieldOfView) << std::endl;
        printed = true;
    }
}

}

