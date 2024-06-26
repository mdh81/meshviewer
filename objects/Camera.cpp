#include "Camera.h"
#include "Types.h"
#include "EventHandler.h"
#include "ConfigurationReader.h"
#include "arcball/ArcballController.h"
#include "Util.h"
#include <iostream>
#include <memory>
#include <filesystem>
using namespace std;

namespace mv {

using namespace common;
using namespace events;

 Camera::Camera(Renderable const& renderable, ProjectionType const type)
    : renderable(renderable)
    , projectionType(type)
    , orbitOn(false)
    , modelTransform(glm::mat4(1.0))
    , viewTransform(glm::mat4(1.0))
    , projectionTransform(glm::mat4(1.0))
    , orbitAngle(0.f)
    , zoomFactor(1.f)
    , movementIncrement(.02f)
    , rotation(math3d::RotationMatrix<float>{}){

     EventHandler().registerBasicEventCallback(GLFW_KEY_O, *this, &Camera::setProjectionType,
                                               ProjectionType::Orthographic);

     EventHandler().registerBasicEventCallback(GLFW_KEY_P, *this, &Camera::setProjectionType,
                                               ProjectionType::Perspective);

     EventHandler().registerBasicEventCallback(GLFW_KEY_X, *this, &Camera::toggleOrbit, common::Axis::X);

     EventHandler().registerBasicEventCallback(GLFW_KEY_Y, *this, &Camera::toggleOrbit, common::Axis::Y);

     EventHandler().registerBasicEventCallback(GLFW_KEY_Z, *this, &Camera::toggleOrbit, common::Axis::Z);
}

void Camera::apply() {
    buildViewTransform();
    buildProjectionTransform();
}

Camera::~Camera() {
    if (timerThread) {
        orbitOn = false;
        timerThread->join();
    }
}

void Camera::buildViewTransform() {
    // Converts the renderable vertices from world coordinate system to
    // camera coordinate system. Camera in OpenGL is fixed at global origin looking
    // down the -Z axis. Moving the model, so it is within the frame of the camera
    // is accomplished by the view transform

    // Get the centroid of the renderable
    // Translate the renderable to world origin
    auto translateToOrigin = glm::mat4(1.0f);
    common::Point3D centroid = renderable.getCentroid();
    translateToOrigin[3] = glm::vec4(-centroid.x, -centroid.y, -centroid.z, 1.f);

    // Rotate about orbit axis by the negative of the orbit angle
    // Use negative angle because we want to simulate a positive rotation of the camera,
    // which means the object has to rotate in the opposite direction
    auto orbit = glm::mat4(1.0f);
    if (orbitAxis) {
        switch (orbitAxis.value()) {
            case Axis::X:
                orbit[1] = glm::vec4(0, glm::cos(glm::radians(-orbitAngle)), glm::sin(glm::radians(-orbitAngle)), 0);
                orbit[2] = glm::vec4(0, -glm::sin(glm::radians(-orbitAngle)), glm::cos(glm::radians(-orbitAngle)), 0);
                break;
            case Axis::Y:
                orbit[0] = glm::vec4(glm::cos(glm::radians(-orbitAngle)), 0, -glm::sin(glm::radians(-orbitAngle)), 0);
                orbit[2] = glm::vec4(-glm::sin(glm::radians(-orbitAngle)), 0, -glm::cos(glm::radians(-orbitAngle)), 0);
                break;
            case Axis::Z:
                orbit[0] = glm::vec4(glm::cos(glm::radians(-orbitAngle)), glm::sin(glm::radians(-orbitAngle)), 0, 0);
                orbit[1] = glm::vec4(-glm::sin(glm::radians(-orbitAngle)), glm::cos(glm::radians(-orbitAngle)), 0, 0);
                break;
            default:
                throw std::runtime_error("Orbiting about arbitrary axis is not supported");
        }
    }

    // Pan the view
    glm::mat4 pan = glm::mat4(1.0);
    pan[3] = glm::vec4(panTarget.x, panTarget.y, 0.f, 1.f);

    auto viewBounds = renderable.getBounds();
    viewBounds.x.min *= zoomFactor;
    viewBounds.y.min *= zoomFactor;
    viewBounds.z.min *= zoomFactor;
    viewBounds.x.max *= zoomFactor;
    viewBounds.y.max *= zoomFactor;
    viewBounds.z.max *= zoomFactor;

    // Translate along -Z by the length of the diagonal of the renderable's bounding box
    // to ensure that the renderable is behind the camera (fixed at 0,0,0)
    auto moveBack = glm::mat4(1.0f);
    moveBack[3] = glm::vec4(0, 0, -viewBounds.length(), 1);

    // Compute the view volume. The volume should be large enough to prevent the renderable
    // from being clipped against its planes in all orientations of the renderable. To get this
    // we build a "super" bounding box, which is a symmetric bounding box whose side length
    // is equal to the diagonal length of the renderable's original bounding box
    viewVolume = Util::transformBounds(Bounds(viewBounds.length()), moveBack);

    static bool printed = false;
    if (!printed && debug) {
        cout << "Renderable Centroid: " << centroid << endl;
        cout << "Renderable Bounds: " << renderable.getBounds() << endl;
        cout << "Moving the renderable back along Z by " << renderable.getBounds().length() << endl;
        cout << "Translate To Origin" << endl;
        Util::printMatrix(translateToOrigin);
        cout << "Orbit" << endl;
        Util::printMatrix(orbit);
        cout << "Move back" << endl;
        Util::printMatrix(moveBack);
        printed = true;
        cout << "ViewTransform" << endl;
        Util::printMatrix(viewTransform);
    }

    // Combine the translations and orbit to produce the view transform
    math3d::Vector<float, 4> row0 = rotation(0);
    math3d::Vector<float, 4> row1 = rotation(1);
    math3d::Vector<float, 4> row2 = rotation(2);
    math3d::Vector<float, 4> row3 = rotation(3);
    glm::mat4 rot (row0[0], row1[0], row2[0], row3[0],
                   row0[1], row1[1], row2[1], row3[1],
                   row0[2], row1[2], row2[2], row3[2],
                   row0[3], row1[3], row2[3], row3[3]);
    //cout << "Rotation matrix" << endl;
    //Util::printMatrix(rot);
    viewTransform = moveBack * pan * orbit * rot * translateToOrigin;
}

void Camera::buildProjectionTransform() {

    if (projectionType == ProjectionType::Orthographic) {
        buildOrthographicProjectionTransform();
    } else {
        buildPerspectiveProjectionTransform();
    }

}

void Camera::buildOrthographicProjectionTransform() {

    float viewWidth = viewVolume.length();
    float viewHeight = viewWidth / renderable.getAspectRatio();

    float viewMinX = -viewWidth  * 0.5f;
    float viewMaxX =  viewWidth  * 0.5f;
    float viewMinY = -viewHeight * 0.5f;
    float viewMaxY =  viewHeight * 0.5f;
    float nearDist =  fabs(viewVolume.z.max);
    float farDist  =  fabs(viewVolume.z.min);

    projectionTransform = glm::ortho(viewMinX, viewMaxX,
                                     viewMinY, viewMaxY,
                                     nearDist, farDist);

    static bool printed = false;
    if (!printed && debug) {
        auto bounds = renderable.getBounds();
        Util::printMatrix(viewTransform);
        cout << "Mesh Bounds " << bounds << endl;
        cout << "View Volume " << viewVolume << endl;
        cout << "Left: " << viewMinX << " Right: " << viewMaxX
                         << " Bottom: " << viewMinY << " Top: " << viewMaxY
                         << " Near: " << nearDist
                         << " Far: " << farDist
                         << endl;
        printed = true;
    }
    writeViewConfigurationToFile();
}

void Camera::buildPerspectiveProjectionTransform() {
    // Compute the field of view

    // The view transform in getViewTransform() has placed the renderable at a distance
    // of d = distance to the near plane of the view volume from the camera.
    // The camera is always at 0,0,0
    //
    // Side-view of the camera and the renderable
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
    // BDEF is a symmetric bounding box of the renderable, whose edge length is
    // the diagonal length of the renderable's box. We choose this bounding
    // box to prevent the renderable from being clipped against the clipping planes
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

    auto AC = fabs(viewVolume.z.max);
    auto BC = viewVolume.y.length() * 0.5f;
    auto fieldOfView = 2.f * glm::atan(BC/AC);
    projectionTransform =
        glm::perspective(fieldOfView, renderable.getAspectRatio(),
                         AC, viewVolume.z.min);

    static bool printed = false;
    if (!printed && debug) {
        cout << "AC = " << AC << endl;
        cout << "BC = " << BC << endl;
        cout << "Fov = " << glm::degrees(fieldOfView) << endl;
        printed = true;
    }
    writeViewConfigurationToFile();
}

void Camera::writeViewConfigurationToFile() {
     if (debug) {
         auto bounds = renderable.getBounds();
         config::ConfigurationReader &cfgReader = config::ConfigurationReader::getInstance();
         std::filesystem::path tmpDir(cfgReader.getValue("temporaryFilesDirectory"));
         if (!std::filesystem::exists(tmpDir)) {
             std::filesystem::create_directory(tmpDir);
         }
         // TODO: Break this feature temporarily until Camera's matrices can be replaced with 3dmath
         renderable.writeToFile(tmpDir / "viewTransformed.stl", {});
         Util::writeBounds(tmpDir / "objectBounds.stl", renderable.getBounds());
         Util::writeBounds(tmpDir / "viewBounds.stl", viewVolume);
     }
}

void Camera::orbitLoop() {
    using namespace std::chrono_literals;
    using namespace std::this_thread;
    while (orbitOn) {
        this->orbitAngle += 0.5;
        if (this->orbitAngle > 360) this->orbitAngle = 0;
        sleep_for(20ms);
    }
}

void Camera::toggleOrbit(const common::Axis& axis) {
    if (!orbitAxis) {
        // If the orbit axis hasn't been set yet, set it and start orbiting around it
        orbitAxis = axis;
        orbitOn = true;
    } else if (axis == orbitAxis) {
        // Toggle activated (same orbit axis). Start or stop orbiting
        orbitOn = !orbitOn;
    } else {
        // Start orbiting around a new axis
        orbitOn = true;
        orbitAxis = axis;
    }
    // Start orbiting
    if (orbitOn) {
        // If we were previously orbiting, then we need to wait for that thread to join
        // before creating a new thread
        if (timerThread) {
            orbitOn = false;
            timerThread->join();
            orbitOn = true;
        }
        timerThread = std::make_unique<std::thread>(&Camera::orbitLoop, this);
    }
}

// TODO: Implement zoom to cursor location
void Camera::zoom(common::Direction direction) {
    switch (direction) {
        case common::Direction::Backward:
            zoomFactor += movementIncrement;
            break;
        case common::Direction::Forward:
            zoomFactor -= movementIncrement;
            // TODO: Don't clamp the zoom factor. This prevents the viewer from reaching
            // all areas of the model. Walkthrough like interaction is prevented by this
            // clamping
            zoomFactor = std::max(zoomFactor, 1e-6f);
            break;
        default:
            throw std::runtime_error("Camera zoom direction is invalid. Zoom operates only forward or backward");
    }
}

 void Camera::pan(common::Direction direction) {
     switch (direction) {
         case common::Direction::Left:
             panTarget.x -= movementIncrement;
             break;
         case common::Direction::Right:
             panTarget.x += movementIncrement;
             break;
         case common::Direction::Up:
             panTarget.y += movementIncrement;
             break;
         case common::Direction::Down:
             panTarget.y -= movementIncrement;
             break;
         default:
             throw std::runtime_error("Camera pan direction is invalid. Pan operates only left, right, up or down");
     }
 }

}
