#ifndef MESH_VIEWER_MESH_H
#define MESH_VIEWER_MESH_H
#include <memory>
#include <initializer_list>
#include <array>
#include <vector>
#include <optional>
#include "Types.h"
#include "MeshViewerObject.h"
#include "Octree.h"

namespace meshviewer {

class Mesh : public MeshViewerObject {
    public:
        Mesh() = default;

        ~Mesh() = default;

        //TODO: Define copy and move semantics

    public:
        // TODO: Allow creation of meshes without this call. Make addVertex
        // and addFace allocate memory as needed. Come up with resonable
        // default initial allocation
        void initialize(const unsigned numVerts, const unsigned numFaces);

        void addVertex(const float x, const float y, const float z);

        void addFace(const std::initializer_list<unsigned>& vertexIds);

        void removeDuplicateVertices();

        unsigned getNumberOfVertices() const { return m_numVertices; }

        unsigned getNumberOfFaces() const { return m_numFaces; }

        const common::Vertices& getVertices() const { return m_vertices; }

        const common::Faces& getConnectivity() const { return m_faces; }

        const common::Vertex& getVertex(unsigned vertexIndex) const;

        const common::Face& getFace(unsigned faceIndex) const;

        const common::Bounds& getBounds() const;

        common::Vertex getCentroid() const;

        void getVertexData(size_t& numBytes, common::Vertex*& vertexData) const;

        void getConnectivityData(size_t& numBytes, unsigned*& connData) const;

    private:
        unsigned m_numVertices;
        unsigned m_numFaces;
        common::Vertices m_vertices;
        common::Faces m_faces;
        std::optional<common::Bounds> m_bounds;
        std::optional<Octree> m_octree;
        std::unique_ptr<unsigned> m_connectivity;
        size_t m_connectivityDataSize;

    private:
        void buildConnectivityData();
        void buildBounds();
};

}

#endif