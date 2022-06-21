#ifndef MESH_VIEWER_PERSPECTIVE_CAMERA_H
#define MESH_VIEWER_PERSPECTIVE_CAMERA_H

#include "Camera.h"

namespace meshviewer {

class PerspectiveCamera : public Camera {
    public:
        PerspectiveCamera(const Mesh& m) : Camera(m) {
        }
        ~PerspectiveCamera() = default;
        virtual void buildProjectionTransform() override;
        void zoomIn() override;
        void zoomOut() override;

    private:
        float m_fieldOfView;
};
}


#endif