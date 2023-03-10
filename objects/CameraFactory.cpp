#include "CameraFactory.h"

namespace mv {

using namespace common;

CameraFactory::CameraFactory() {
    m_mesh = nullptr; 
}

Camera& CameraFactory::getCamera(const Mesh& mesh, const WindowDimensions& winDim) {
    bool createNew = !m_mesh || *m_mesh != mesh; 
    if (createNew) {
        m_camera.reset(new Camera(mesh, winDim));
        m_mesh = &mesh;
    }
    return *m_camera.get();
}

}
