#pragma once

#include <memory>
#include "GL/glew.h"
#include "Renderable.h"
#include "Types.h"
#include "glm/matrix.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>
#include <thread>
#include <optional>

namespace mv {

class Drawable;

class Camera : public MeshViewerObject {

    public:
        enum class ProjectionType {
                Orthographic,
                Perspective
        };

        // Cameras are meant to be shared across renderables and across viewports
        using SharedCameraPointer = std::shared_ptr<Camera>;

    public:
        explicit Camera(Renderable const& renderable, ProjectionType type = ProjectionType::Perspective);
        ~Camera();

        // Applies the camera parameters and generates a view
        void apply();

        // Makes the camera orbit around the specified axis
        void setOrbitOn(const common::Axis& axis) { toggleOrbit(axis); }
        void setOrbitOff() { orbitOn = false; }

        // Zooms the camera in and out
        void zoomIn();
        void zoomOut();

        // Sets projection type
        void setProjectionType(const ProjectionType type) { projectionType = type; }

        [[nodiscard]] glm::mat4 getViewTransform() const { return viewTransform; }

        [[nodiscard]] glm::mat4 getProjectionTransform() const { return projectionTransform; }

        [[nodiscard]] common::Bounds getViewVolume() const { return viewVolume; }

private:
        void buildViewTransform();
        void buildProjectionTransform();
        void buildPerspectiveProjectionTransform();
        void buildOrthographicProjectionTransform();
        void toggleOrbit(const common::Axis& axis);
        void orbitLoop();

    private:
        std::optional<common::Axis> orbitAxis;
        bool orbitOn;
        glm::mat4 modelTransform;
        glm::mat4 viewTransform;
        glm::mat4 projectionTransform;
        ProjectionType projectionType;
        float orbitAngle;
        std::unique_ptr<std::thread> timerThread;
        common::Bounds viewVolume;
        Renderable const& renderable;
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