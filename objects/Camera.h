#ifndef MESH_VIEWER_CAMERA_H
#define MESH_VIEWER_CAMERA_H

#include <memory>
#include "GL/glew.h"
#include "MeshViewerObject.h"
#include "Types.h"
#include "glm/matrix.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>
#include <thread>
#include <optional>

namespace mv {

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
        ~Camera();

        // Applies the camera parameters and generates a view
        void apply();

        // Makes the camera orbit around the specified axis
        void setOrbitOn(const common::Axis& axis) { toggleOrbit(axis); }
        void setOrbitOff() { m_orbitOn = false; }

        // Zooms the camera in and out
        void zoomIn();
        void zoomOut();

        // Sets projection type
        void setProjectionType(const ProjectionType type) { m_projectionType = type; }

        glm::mat4 getViewTransform() const { return m_viewTransform; }

        glm::mat4 getProjectionTransform() const { return m_projectionTransform; }

    private:
        void buildViewTransform();
        void buildProjectionTransform();
        void buildPerspectiveProjectionTransform();
        void buildOrthographicProjectionTransform();
        void toggleOrbit(const common::Axis& axis);
        void orbitLoop();
        glm::mat4 rotateAboutX(float angleInDegrees);
        glm::mat4 rotateAboutY(float angleInDegrees);
        glm::mat4 rotateAboutZ(float angleInDegrees);

    private:
        const Mesh& m_mesh;
        std::optional<common::Axis> m_orbitAxis;
        bool m_orbitOn;
        glm::mat4 m_modelTransform;
        glm::mat4 m_viewTransform;
        glm::mat4 m_projectionTransform;
        ProjectionType m_projectionType;
        float m_fieldOfView;
        common::WindowDimensions m_windowDimensions;
        float m_orbitAngle;
        std::unique_ptr<std::thread> m_timerThread;
        common::Bounds m_viewVolume;
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
