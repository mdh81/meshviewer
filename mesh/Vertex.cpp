#include "Vertex.h"
#include "Mesh.h"

namespace meshviewer {

// Compute vertex normal as the average of normals of all faces that
// share that vertex
math3d::Vector<float, 3> Vertex::getNormal(const Mesh& mesh) const {
    math3d::Vector<float,3> normal;
    for (size_t i = 0; i < m_faces.size(); ++i) {
        const Face& face = mesh.getFace(m_faces.at(i));
        normal += face.getNormal(mesh);
    }
    normal /= m_faces.size();
    return normal.normalize();
}

}
