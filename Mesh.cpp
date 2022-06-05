#include "Mesh.h"
#include <limits>
#include <iostream>
using namespace std;

namespace meshviewer {

using namespace common;

void Mesh::initialize(const unsigned numVerts, const unsigned numFaces) {
    m_numVertices = numVerts;
    m_numFaces = numFaces;
    m_vertices.reserve(m_numVertices);
    m_faces.reserve(m_numFaces);
}

void Mesh::addVertex(const float x, const float y, const float z) {
    m_vertices.push_back({x, y, z});
}

void Mesh::addFace(const initializer_list<unsigned>& vertexIds) {
    m_faces.emplace_back(vertexIds);
}

const Vertex& Mesh::getVertex(unsigned vertexIndex) const {
    if (vertexIndex >= m_numVertices)
        throw std::runtime_error("Vertex index invalid");
    return m_vertices.at(vertexIndex);
} 

const Face& Mesh::getFace(unsigned faceIndex) const {
    if (faceIndex >= m_numFaces)
        throw std::runtime_error("Vertex index invalid");
    return m_faces.at(faceIndex);
}

const Bounds& Mesh::getBounds() {
    if (!m_bounds) {
        m_bounds.emplace();
        for (auto& v : m_vertices) {
            if (v.x < m_bounds->xmin) m_bounds->xmin = v.x;
            if (v.y < m_bounds->ymin) m_bounds->ymin = v.y;
            if (v.z < m_bounds->zmin) m_bounds->zmin = v.z;

            if (v.x > m_bounds->xmax) m_bounds->xmax = v.x;
            if (v.y > m_bounds->ymax) m_bounds->ymax = v.y;
            if (v.z > m_bounds->zmax) m_bounds->zmax = v.z;
        }
    }
    return m_bounds.value();
}

void Mesh::removeDuplicateVertices() {
    if (!m_octree) {
        m_octree.emplace(*this);
    }
    for (auto& v : m_vertices) {

    }
}

Vertex Mesh::getCentroid() {
    if (!m_bounds) {
        getBounds();
    }
    return Vertex { (this->m_bounds->xmax + this->m_bounds->xmin) * 0.5f,
                    (this->m_bounds->ymax + this->m_bounds->ymin) * 0.5f,
                    (this->m_bounds->zmax + this->m_bounds->zmin) * 0.5f };
}

}
