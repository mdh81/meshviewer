#include "Camera.h"
#include "Mesh.h"
#include "Types.h"
#include "EventHandler.h"
#include "CallbackFactory.h"
#include "Util.h"
#include <iostream>
#include <exception>
using namespace std;

namespace meshviewer {

using namespace common;
using namespace events;

Camera::Camera(const Mesh& m, const WindowDimensions& winDim, const ProjectionType type)
   : m_mesh(m)
   , m_windowDimensions(winDim)
   , m_projectionType(type)
   , m_orbitOn(false) {

    // Register event handlers for switching between perspective and orthographic 
    EventHandler().registerCallback(Event(GLFW_KEY_O), 
        CallbackFactory::getInstance().registerCallback(
            *this, &Camera::setProjectionType, ProjectionType::Orthographic));

    EventHandler().registerCallback(Event(GLFW_KEY_P), 
        CallbackFactory::getInstance().registerCallback(
            *this, &Camera::setProjectionType, ProjectionType::Perspective));

    EventHandler().registerCallback(Event(GLFW_KEY_X),
        CallbackFactory::getInstance().registerCallback(
            *this, &Camera::toggleOrbit, common::Axis::X));


    EventHandler().registerCallback(Event(GLFW_KEY_Y),
        CallbackFactory::getInstance().registerCallback(
            *this, &Camera::toggleOrbit, common::Axis::Y));


    EventHandler().registerCallback(Event(GLFW_KEY_Z),
        CallbackFactory::getInstance().registerCallback(
            *this, &Camera::toggleOrbit, common::Axis::Z));
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

Camera::~Camera() {
    if (m_timerThread) {
        m_orbitOn = false;
        m_timerThread->join();
    }
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

    // Rotate about orbit axis by the negative of the orbit angle 
    // Use negative angle because we want to simulate a positive rotation of the camera,
    // which means the object has to rotate in the opposite direction
    auto orbit = glm::mat4(1.0f);
    if (m_orbitOn) {
        switch (m_orbitAxis.value()) {
            case Axis::X:
                orbit[1] = glm::vec4(0, glm::cos(glm::radians(-m_orbitAngle)), glm::sin(glm::radians(-m_orbitAngle)), 0); 
                orbit[2] = glm::vec4(0, -glm::sin(glm::radians(-m_orbitAngle)), glm::cos(glm::radians(-m_orbitAngle)), 0);
                break;
            case Axis::Y:
                orbit[0] = glm::vec4(glm::cos(glm::radians(-m_orbitAngle)), 0, -glm::sin(glm::radians(-m_orbitAngle)), 0); 
                orbit[2] = glm::vec4(-glm::sin(glm::radians(-m_orbitAngle)), 0, -glm::cos(glm::radians(-m_orbitAngle)), 0);
                break;
            case Axis::Z:
                orbit[0] = glm::vec4(glm::cos(glm::radians(-m_orbitAngle)), glm::sin(glm::radians(-m_orbitAngle)), 0, 0); 
                orbit[1] = glm::vec4(-glm::sin(glm::radians(-m_orbitAngle)), glm::cos(glm::radians(-m_orbitAngle)), 0, 0);
                break;
            default:
                throw std::runtime_error("Orbiting about arbitrary axis is not supported");
        }
    }

    // Translate along -Z by the length of the diagonal of the mesh's bounding box 
    // to ensure that the mesh is behind the camera (fixed at 0,0,0)
    auto moveBack = glm::mat4(1.0f);
    moveBack[3] = glm::vec4(0, 0, -m_mesh.getBounds().len(), 1);
    
    // Compute the view volume. The volume should be large enough to prevent the mesh
    // from being clipped against its planes in all orientations of the mesh. To get this
    // we build a "super" bounding box, which is a symmetric bounding box whose side length
    // is equal to the diagonal length of the mesh's original bounding box
    m_viewVolume = Util::transformBounds(Bounds(m_mesh.getBounds().len()), moveBack); 

    static bool printed = false;
    if (!printed && m_debugOn) {
        std::cout << "Mesh Centroid: " << centroid << std::endl;
        std::cout << "Mesh Bounds: " << m_mesh.getBounds() << std::endl;
        std::cout << "Moving the mesh back along Z by " << m_mesh.getBounds().len() << std::endl;
        cout << "Translate To Origin" << endl;
        Util::printMatrix(translateToOrigin);
        cout << "Orbit" << endl;
        Util::printMatrix(orbit);
        cout << "Move back" << endl;
        Util::printMatrix(moveBack);
        printed = true;
    }

    // Combine the translations and orbit to produce the view transform 
    m_viewTransform = moveBack * orbit * translateToOrigin;
}

void Camera::buildProjectionTransform() {
    
    if (m_projectionType == ProjectionType::Orthographic) {
        buildOrthographicProjectionTransform();
    } else {
        buildPerspectiveProjectionTransform();
    }

}

void Camera::buildOrthographicProjectionTransform() {

    float viewWidth = m_viewVolume.len(); 
    float viewHeight = (viewWidth * m_windowDimensions.height/m_windowDimensions.width);

    float viewMinX = -viewWidth  * 0.5;
    float viewMaxX =  viewWidth  * 0.5;
    float viewMinY = -viewHeight * 0.5;
    float viewMaxY =  viewHeight * 0.5;
    float nearDist =  fabs(m_viewVolume.zmax);
    float farDist  =  fabs(m_viewVolume.zmin);

    m_projectionTransform = glm::ortho(viewMinX, viewMaxX,
                                       viewMinY, viewMaxY,
                                       nearDist, farDist);
    
    static bool printed = false;
    if (!printed && m_debugOn) {
        auto bounds = m_mesh.getBounds();
        m_mesh.transform(m_viewTransform)->writeToSTL("/Users/deye/viewer/build/viewTransformed.stl");
        Util::printMatrix(m_viewTransform); 
        std::cout << "Mesh Bounds " << bounds << std::endl;
        std::cout << "View Volume " << m_viewVolume << std::endl;
        std::cout << "Left: " << viewMinX << " Right: " << viewMaxX 
                  << " Bottom: " << viewMinY << " Top: " << viewMaxY 
                  << " Near: " << nearDist 
                  << " Far: " << farDist
                  << std::endl;
        Util::writeBounds("/Users/deye/viewer/build/objectBounds.stl", m_mesh.getBounds());
        Util::writeBounds("/Users/deye/viewer/build/viewBounds.stl", m_viewVolume);
        printed = true;
    }
}

void Camera::buildPerspectiveProjectionTransform() {
    // Compute the field of view
    
    // The view transform in getViewTransform() has placed the mesh at a distance of
    // of d = distance to the near plane of the view volume from the camera
    // The camera is always at 0,0,0
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
    // BDEF is a symmetric bounding box of the mesh, whose edge length is
    // the diagonal length of the mesh's box. We choose this bounding
    // box to prevent the mesh from being clipped against the clipping planes
    // during view transformations
    //
    //
    // Fov is the angle between AB and AD
    // 
    // To find this angle, we consider the triangle ABC
    // Angle between AC and BC is 90 degrees. The angle
    // between AC and AB is Fov/2
    //
    // BC = Opposite side = 0.5 * Length of bounding box  
    // AC = Adjacent side = Length of bounding box 
    // tan(fov/2) = BC/AC
    // fov/2 = arctan(BC/AC)
    // fov = 2 * arctan(BC/AC)


    auto AC = fabs(m_viewVolume.zmax);
    auto BC = m_viewVolume.ylen() * 0.5f;
    m_fieldOfView = 2.f * glm::atan(BC/AC);
    m_projectionTransform = 
        glm::perspective(m_fieldOfView, 
                         static_cast<float>(m_windowDimensions.width)/m_windowDimensions.height,
                         AC, m_viewVolume.zmin);

    static bool printed = false;
    if (!printed && m_debugOn) {
        std::cout << "AC = " << AC << std::endl;
        std::cout << "BC = " << BC << std::endl;
        std::cout << "Fov = " << glm::degrees(m_fieldOfView) << std::endl;
        printed = true;
    }
}

void Camera::orbitLoop() {
    using namespace std::chrono_literals;
    using namespace std::this_thread;
    while (m_orbitOn) {
        this->m_orbitAngle += 0.5;
        if (this->m_orbitAngle > 360) this->m_orbitAngle = 0;   
        sleep_for(20ms);
    }
}

void Camera::toggleOrbit(const common::Axis& axis) { 
    if (!m_orbitAxis) {
        // If the orbit axis hasn't been set yet, set it and start orbiting around it
        m_orbitAxis = axis;
        m_orbitOn = true;
    } else if (axis == m_orbitAxis) {
        // Toggle activated (same orbit axis). Start or stop orbiting
        m_orbitOn = !m_orbitOn;
    } else {
        // Start orbiting around a new axis
        m_orbitOn = true;
        m_orbitAxis = axis;
    }
    // Start orbitting
    if (m_orbitOn) {
        // If we were previously orbitting, then we need to wait for that thread to join 
        // before creating a new thread
        if (m_timerThread) { 
            m_orbitOn = false;
            m_timerThread->join();
            m_orbitOn = true;
        }
        m_timerThread.reset(new std::thread(&Camera::orbitLoop, this));
    }
} 

}
