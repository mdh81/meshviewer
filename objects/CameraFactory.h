#ifndef MESH_VIEWER_CAMERA_FACTORY_H
#define MESH_VIEWER_CAMERA_FACTORY_H

#include "Camera.h"
#include "Mesh.h"
#include "Types.h"

#include <memory>

namespace mv {

class CameraFactory {
    public:
        static CameraFactory& getInstance() {
            static CameraFactory instance;
            return instance;
        }
        Camera& getCamera(const Mesh&, const common::WindowDimensions& winDim); 

    private:
       std::unique_ptr<Camera> m_camera; 
       const Mesh* m_mesh;

    private:
        CameraFactory();
        CameraFactory(const CameraFactory&) = delete;
        CameraFactory(CameraFactory&&) = delete;
        CameraFactory& operator=(const CameraFactory&) = delete;
        CameraFactory& operator=(CameraFactory&&) = delete;
};

}

#endif
