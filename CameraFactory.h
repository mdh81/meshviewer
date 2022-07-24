#ifndef MESH_VIEWER_CAMERA_FACTORY_H
#define MESH_VIEWER_CAMERA_FACTORY_H

#include "Camera.h"
#include "Mesh.h"

#include <memory>
#include <iostream>

namespace meshviewer {

class CameraFactory {
    public:
        enum class ProjectionType {
            Orthographic,
            Perspective
        }; 
        
        static CameraFactory& getInstance() {
            static CameraFactory instance;
            return instance;
        }

    public:
        Camera& getCamera(const Mesh&, const ProjectionType); 
        void setProjectionType(const ProjectionType);

    private:
       std::unique_ptr<Camera> m_camera; 
       const Mesh* m_mesh;
       ProjectionType m_type;

    private:
        CameraFactory();
        CameraFactory(const CameraFactory&) = delete;
        CameraFactory(CameraFactory&&) = delete;
        CameraFactory& operator=(const CameraFactory&) = delete;
        CameraFactory& operator=(CameraFactory&&) = delete;
};

inline std::ostream& operator<<(std::ostream& os, CameraFactory::ProjectionType p) {
    if (p == CameraFactory::ProjectionType::Orthographic) {
        os << "Orthographic";
    } else {
        os << "Perspective";
    }
    return os;
}

}

#endif
