#ifndef MESH_VIEWER_ORTHOGRAPHIC_CAMERA_H
#define MESH_VIEWER_ORTHOGRAPHIC_CAMERA_H

#include "Camera.h"

namespace meshviewer {

class OrthographicCamera : public Camera {
    public:
        OrthographicCamera(const Mesh& m) : Camera(m) { 
        }
        ~OrthographicCamera() = default;
        void buildProjectionTransform() override;
        void zoomIn() override;
        void zoomOut() override;
};

}


#endif