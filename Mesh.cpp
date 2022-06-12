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

void Mesh::getVertexData(size_t& numBytes, common::Vertex*& pVertexData) const {
    numBytes = m_numVertices * sizeof(common::Vertex);
    pVertexData = const_cast<common::Vertex*>(m_vertices.data());
}

void Mesh::buildConnectivityData() {
    m_connectivityDataSize = 0;
    for (size_t i = 0; i < m_numFaces; ++i) {
        m_connectivityDataSize += (m_faces.at(i).size() * sizeof(unsigned));
    }
    m_connectivity.reset(new unsigned[m_connectivityDataSize]); 
    for (size_t i = 0, offset = 0; i < m_numFaces; ++i) {
        size_t faceDataSize = m_faces.at(i).size() * sizeof(unsigned);
        memcpy(m_connectivity.get() + offset, m_faces.at(i).data(), faceDataSize);
        offset += m_faces.at(i).size();
    }
}

void Mesh::getConnectivityData(size_t& numBytes, unsigned*& pConnData) const {
    // C++ doesn't provide a way out of const cast to support lazy loading that's
    // necessary here. Not all meshes will be rendered and until a mesh is rendered
    // there is no need to get connectivity data out of a mesh like below
    if (!m_connectivity) const_cast<Mesh*>(this)->buildConnectivityData();
    numBytes = m_connectivityDataSize;
    pConnData = m_connectivity.get(); 
}


}
