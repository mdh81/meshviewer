#include "CameraFactory.h"

namespace meshviewer {

CameraFactory::CameraFactory() {
    m_mesh = nullptr; 
}

Camera& CameraFactory::getCamera(const Mesh& mesh) {
    bool createNew = !m_mesh || *m_mesh != mesh; 
    if (createNew) {
        m_camera.reset(new Camera(mesh));
        m_mesh = &mesh;
    }
    return *m_camera.get();
}

}
