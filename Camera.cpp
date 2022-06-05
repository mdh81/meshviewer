#include "Camera.h"
#include "Mesh.h"
#include "Types.h"
#include "glm/matrix.hpp"

namespace meshviewer {

using namespace common;

Camera::Camera(const Mesh& mesh, const ProjectionType projectionType) {
    if (projectionType == ProjectionType::Perspective) {
        m_projection.reset(new PerspectiveProjection(mesh)); 
    } else {
        m_projection.reset(new OrthographicProjection(mesh));
    }
}

Camera::Projection::Projection(const Mesh& mesh) 
    : m_mesh(mesh) {

}

// This method computes the translation matrix necesssary to translate the model to the
// center of the global system
void Camera::Projection::fitViewToModel() {
    
    

}

Camera::PerspectiveProjection::PerspectiveProjection(const Mesh& mesh) 
    : Camera::Projection(mesh) {

}

Camera::OrthographicProjection::OrthographicProjection(const Mesh& mesh) 
    : Camera::Projection(mesh) {

}

void Camera::PerspectiveProjection::setupView() {

}

void Camera::OrthographicProjection::setupView() {

}


}

