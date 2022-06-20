#ifndef MESH_VIEWER_CAMERA_FACTORY_H
#define MESH_VIEWER_CAMERA_FACTORY_H

#include "Camera.h"
#include "PerspectiveCamera.h"
#include "OrthographicCamera.h"

#include <memory>

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
        Camera& getCamera(const Mesh& mesh, const ProjectionType projType) {
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
    private:
       std::unique_ptr<Camera> m_camera; 
       const Mesh* m_mesh;
       ProjectionType m_type;

    private:
        CameraFactory() {m_mesh = nullptr;};
        CameraFactory(const CameraFactory&) = delete;
        CameraFactory(CameraFactory&&) = delete;
        CameraFactory& operator=(const CameraFactory&) = delete;
        CameraFactory& operator=(CameraFactory&&) = delete;
};

}

#endif