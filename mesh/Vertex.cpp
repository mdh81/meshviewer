#include "Vertex.h"
#include "Mesh.h"

namespace mv {

using namespace common;

// Compute vertex normal as the average of normals of all faces that
// share that vertex
Vector3D Vertex::getNormal(const Mesh& mesh) const {
    Vector3D normal;
    for (size_t i = 0; i < m_faces.size(); ++i) {
        const Face& face = mesh.getFace(m_faces.at(i));
        normal += face.getNormal(mesh);
    }
    normal /= m_faces.size();
    return normal.normalize();
}

}
