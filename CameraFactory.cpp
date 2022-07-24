#include "CameraFactory.h"
#include "PerspectiveCamera.h"
#include "OrthographicCamera.h"
#include "CallbackFactory.h"
#include "EventHandler.h"

#include <memory>
#include <iostream>
using namespace std;
using namespace meshviewer::events;

namespace meshviewer {

CameraFactory::CameraFactory() {
    m_mesh = nullptr;
    
    // Register event handlers for switching between perspective and orthographic 
    EventHandler::getInstance().registerCallback(KeyEvent(GLFW_KEY_O, KeyEvent::Trigger::Press), 
        CallbackFactory::getInstance().registerCallback(
            *this, &CameraFactory::setProjectionType, ProjectionType::Orthographic));

    EventHandler::getInstance().registerCallback(KeyEvent(GLFW_KEY_P, KeyEvent::Trigger::Press), 
        CallbackFactory::getInstance().registerCallback(
            *this, &CameraFactory::setProjectionType, ProjectionType::Perspective));
}

Camera& CameraFactory::getCamera(const Mesh& mesh, const ProjectionType projType) {
    bool createNew = !m_mesh || m_type != projType || *m_mesh != mesh; 
    if (createNew) {
        if (projType == ProjectionType::Orthographic) {
            m_camera.reset(new OrthographicCamera(mesh));
        } else {
            m_camera.reset(new PerspectiveCamera(mesh));
        }
        m_mesh = &mesh;
        m_type = projType;
    }
    return *m_camera.get();
}

void CameraFactory::setProjectionType(const ProjectionType projType) {
    cout << __FUNCTION__ << ' ' << projType << endl; 
}

}
