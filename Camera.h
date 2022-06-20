#ifndef MESH_VIEWER_CAMERA_H
#define MESH_VIEWER_CAMERA_H

#include <memory>
#include "GL/glew.h"
#include "Types.h"
#include "glm/matrix.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "MeshViewerObject.h"

namespace meshviewer {

class Mesh;
    
class Camera : public MeshViewerObject {
    
    public:
        // TODO: Define creation semantics
        Camera(const Mesh& m) : m_mesh(m) {}
        virtual ~Camera() = default;

        // Applies the camera parameters and generates a view
        void apply(GLuint shaderProgram);
        
        // Makes the camera orbit around the specified axis 
        void setOrbitOn(const common::Axis& axis) { m_orbitAxis = axis; m_orbitOn = true; }
        void setOrbitOff() { m_orbitOn = false; }

        // Zooms the camera in and out
        virtual void zoomIn() = 0;
        virtual void zoomOut() = 0;

    private:
        void getModelTransform(glm::mat4&);
        void getViewTransform(glm::mat4&);
        virtual void getProjectionTransform(glm::mat4&) = 0;

    protected:
        const Mesh& m_mesh;
        common::Axis m_orbitAxis;
        bool m_orbitOn;
};

}

#endif
