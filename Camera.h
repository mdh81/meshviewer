#ifndef CAMERA_H
#define CAMERA_H

#include <memory>
#include "GL/glew.h"
#include "Types.h"
#include "glm/matrix.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace meshviewer {

class Mesh;
    
class Camera {
    
    public:
        enum class ProjectionType {
            Orthographic,
            Perspective
        };

    public:
        // TODO: Define creation semantics
        Camera(const Mesh&, const GLuint shaderProgram, const ProjectionType projType = 
               ProjectionType::Perspective);
        ~Camera() = default;
        
        // Applies the camera parameters and generates a view
        void apply();
        
        // Makes the camera orbit around the specified axis 
        void setOrbitOn(const common::Axis& axis) { m_orbitAxis = axis; m_orbitOn = true; }
        void setOrbitOff() { m_orbitOn = false; }

    private:
        void getModelTransform(glm::mat4&);
        void getViewTransform(glm::mat4&);
        void getProjectionTransform(glm::mat4&);

    private:
        const Mesh& m_mesh;
        const GLuint m_shaderProgram;
        ProjectionType m_projectionType;
        common::Axis m_orbitAxis;
        bool m_orbitOn;
};

}

#endif