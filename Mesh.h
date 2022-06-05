#ifndef MESH_H
#define MESH_H
#include <memory>
#include <initializer_list>
#include <array>
#include <vector>
#include <optional>
#include "Types.h"
#include "Octree.h"

namespace meshviewer {

class Mesh {
    public:
        Mesh() = default;
        ~Mesh() = default;
        //TODO: Define copy and move semantics

    public:
        void initialize(const unsigned numVerts, const unsigned numFaces);
        void addVertex(const float x, const float y, const float z);
        void addFace(const std::initializer_list<unsigned>& vertexIds);
        void removeDuplicateVertices();
        unsigned getNumberOfVertices() const { return m_numVertices; }
        unsigned getNumberOfFaces() const { return m_numFaces; }
        const common::Vertex& getVertex(unsigned vertexIndex) const;
        const common::Face& getFace(unsigned faceIndex) const;
        const common::Bounds& getBounds();
        common::Vertex getCentroid();
        const common::Vertices& getVertices() const { return m_vertices; }
        void getVertexData(size_t& numBytes, common::Vertex** vertexData) const {
            numBytes = m_numVertices * sizeof(common::Vertex);
            *vertexData = const_cast<common::Vertex*>(m_vertices.data());
        }

    private:
        void calculateBounds();

    private:
        unsigned m_numVertices;
        unsigned m_numFaces;
        common::Vertices m_vertices;
        common::Faces m_faces;
        std::optional<common::Bounds> m_bounds;
        std::optional<Octree> m_octree;
};

}

#endif
