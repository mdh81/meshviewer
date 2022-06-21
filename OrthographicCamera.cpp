#include "OrthographicCamera.h"
#include "Mesh.h"
#include "Util.h"
#include <memory>
using namespace std;

namespace meshviewer {

void OrthographicCamera::buildProjectionTransform() {
    
    // Create a view volume that is of the same dimensions as
    // the bounding box of the mesh in X and Y directions.
    // Set near plane to zlen/2 and far plane to near + z-len 
    auto bounds = m_mesh.getBounds();
    auto boundsView = Util::transformBounds(bounds, m_viewTransform);

    m_projectionTransform = glm::ortho(1.5f*boundsView.xmin, 1.5f*boundsView.xmax,
                                       1.5f*boundsView.ymin, 1.5f*boundsView.ymax,
                                       boundsView.zlen()*0.5f, boundsView.zlen()*0.5f + boundsView.zlen());
    
    static bool printed = false;
    if (!printed && m_debugOn) {
        std::cout << "Mesh Bounds: " << boundsView << std::endl;
        std::cout << "Left: " << 1.5f*boundsView.xmin << " Right: " << 1.5f*boundsView.xmax << " Bottom: " << 1.5f*boundsView.ymin 
                  << " Top: " << 1.5f*boundsView.ymax << " Near: " << boundsView.zlen()*0.5f 
                  << " Far: " << boundsView.zlen() + boundsView.zlen() * 0.5f
                  << std::endl;
        printed = true;
        unique_ptr<Mesh> spTransformedMesh(std::move(m_mesh.transform(m_viewTransform)));
        spTransformedMesh->writeToSTL("./ObjectInCameraSystem.stl");
    }


}

void OrthographicCamera::zoomIn() {

}

void OrthographicCamera::zoomOut() {

}

}
