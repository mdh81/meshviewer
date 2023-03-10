#include "Face.h"
#include "Mesh.h"
#include "Types.h"
using namespace math3d;

namespace mv {
using namespace common;

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


void Face::replaceVertex(const unsigned oldIndex, const unsigned newIndex) {

    for (size_t i = 0; i < m_vertexIds.size(); ++i) {
        if (m_vertexIds.at(i) == oldIndex) {
            m_vertexIds.at(i) = newIndex;
        }
    }

}

Point3D Face::getCentroid(const Mesh& mesh) const {
    
    // Value initialize the output 
    Point3D centroid = {}; 
    for (size_t i = 0; i < m_vertexIds.size(); ++i) {
        centroid.x += mesh.getVertex(i).x; 
        centroid.y += mesh.getVertex(i).y;
        centroid.z += mesh.getVertex(i).z; 
    }
    centroid.x /= m_vertexIds.size();
    centroid.y /= m_vertexIds.size();
    centroid.z /= m_vertexIds.size();
    return centroid;
}

}
