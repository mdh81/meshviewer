#ifndef MESH_VIEWER_CAMERA_H
#define MESH_VIEWER_CAMERA_H

#include <memory>
#include "GL/glew.h"
#include "MeshViewerObject.h"
#include "Types.h"
#include "glm/matrix.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>

namespace meshviewer {

class Mesh;
    
class Camera : public MeshViewerObject {

    public:
        enum class ProjectionType {
            Orthographic,
            Perspective
        };
    
    public:
        Camera(const Mesh& m, const common::WindowDimensions& winDim,
               const ProjectionType type = ProjectionType::Perspective);
        ~Camera() = default;

        

        // Applies the camera parameters and generates a view
        void apply(GLuint shaderProgram);

        // Makes the camera orbit around the specified axis 
        void setOrbitOn(const common::Axis& axis) { m_orbitAxis = axis; m_orbitOn = true; }
        void setOrbitOff() { m_orbitOn = false; }

        // Zooms the camera in and out
        void zoomIn();
        void zoomOut();

        // Sets projection type 
        void setProjectionType(const ProjectionType type) { m_projectionType = type; }

    private:
        void buildModelTransform();
        void buildViewTransform();
        void buildProjectionTransform();
        void buildPerspectiveProjectionTransform();
        void buildOrthographicProjectionTransform();

    private:
        const Mesh& m_mesh;
        common::Axis m_orbitAxis;
        bool m_orbitOn;
        glm::mat4 m_modelTransform;
        glm::mat4 m_viewTransform;
        glm::mat4 m_projectionTransform;
        ProjectionType m_projectionType;
        float m_fieldOfView;
        common::WindowDimensions m_windowDimensions;
};

inline std::ostream& operator<<(std::ostream& os, Camera::ProjectionType p) {
    if (p == Camera::ProjectionType::Orthographic) {
        os << "Orthographic";
    } else {
        os << "Perspective";
    }
    return os;
}

}

#endif
