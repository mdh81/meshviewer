#include "PerspectiveCamera.h"
#include "Mesh.h"

namespace meshviewer {

void PerspectiveCamera::getProjectionTransform(glm::mat4& projectionMatrix) {
    // Compute the field of view
    
    // The view transform in getViewTransform() has placed the mesh at a distance of
    // of d = zlen(mesh)/2 from the camera, which is always at 0,0,0
    //
    // Side-view of the camera and the mesh             
    //
    // Camera is at the origin A            
    //                                                              +Y
    //                      B--------------------F                  |
    //                      |                    |                  |
    //                      |                    |                  |
    //      A-------d-------C                    |       +Z_________|
    //   (0,0,0)            |                    |
    //                      |                    |
    //                      D--------------------E
    //
    // Fov is the angle between AB and AD
    // BDEF is the bounding box of the mesh
    // 
    // To find this angle, we consider the triangle ABC
    // Angle between AC and BC is 90 degrees. The angle
    // between AC and AB is Fov/2
    //
    // BC = Opposite side = 0.5 * Y-Len of mesh  
    // AC = Adjacent side = 0.5 * z-Len of mesh 
    // tan(fov/2) = BC/AC
    // fov/2 = arctan(BC/AC)
    // fov = 2 * arctan(BC/AC)

    common::Bounds bounds = m_mesh.getBounds();
    auto AC = bounds.zlen() * 0.5f;
    auto BC = bounds.ylen() * 0.5f;
    m_fieldOfView = 2.f * glm::atan(BC/AC);
    projectionMatrix = glm::perspective(m_fieldOfView, 640.f/480.f, AC, AC + bounds.zlen());

    static bool printed = false;
    if (!printed) {
        std::cout << "Y-Length = " << bounds.ylen() << std::endl;
        std::cout << "Z-Length = " << bounds.zlen() << std::endl;
        std::cout << "AC = " << AC << std::endl;
        std::cout << "BC = " << BC << std::endl;
        std::cout << "Fov = " << glm::degrees(m_fieldOfView) << std::endl;
        printed = true;
    }
}

void PerspectiveCamera::zoomIn() {

}

void PerspectiveCamera::zoomOut() {

}

}