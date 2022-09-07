#include "Face.h"
#include "Mesh.h"
using namespace math3d;

namespace meshviewer {

math3d::Vector<float, 3> Face::getNormal(const Mesh& mesh) const {
    // TODO: Handle degenerate faces: co-linear, non-planar, etc
            
    if (m_vertexIds.size() < 3) {
        std::cerr << "1D face detected. Skipping from normal computation" << std::endl;
        return math3d::Vector<float,3>();
    }
    
    // Build two edge vectors 
    auto v1 = mesh.getVertex(m_vertexIds[1]) -
              mesh.getVertex(m_vertexIds[0]);

    auto v2 = mesh.getVertex(m_vertexIds[2]) -
              mesh.getVertex(m_vertexIds[1]);
    
    // Compute normal vector as the cross product
    // of the edge vectors
    return (v1 * v2).normalize();
}


}
