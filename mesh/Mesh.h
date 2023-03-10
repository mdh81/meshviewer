#ifndef MESH_VIEWER_MESH_H
#define MESH_VIEWER_MESH_H
#include <memory>
#include <initializer_list>
#include <array>
#include <vector>
#include <optional>
#include "Types.h"
#include "Renderable.h"
#include "Octree.h"
#include "Vertex.h"
#include "Face.h"
#include "glm/glm.hpp"

namespace mv {

class Camera;

class Mesh : public Renderable {
    public:
        using Vertices = std::vector<Vertex>;
        using Faces = std::vector<Face>;
        using NormalData = common::Array<float, 3>;
        using VertexData = common::Array<float, 3>;

    public:
        Mesh();

        ~Mesh() = default;

        Mesh(const Mesh &);

        // TODO: Define copy assignment
        //       Define move assignment
        //       Define move constructor
        //

    public:
        void render(Camera const&) override;

    public:
        // TODO: Allow creation of meshes without this call. Make addVertex
        // and addFace allocate memory as needed. Come up with resonable
        // default initial allocation
        void initialize(const unsigned numVertices, const unsigned numFaces);

        // Adds a new vertex to this mesh
        void addVertex(const float x, const float y, const float z);

        // Adds a new face that is described the specified vertex identifiers
        // NOTE: vertexIds are 0-based
        void addFace(const std::initializer_list<unsigned>& vertexIds);

        // Merges coincident vertices and adjusts the connectivity data
        // accordingly. Return number of duplicates that were removed
        unsigned removeDuplicateVertices();

        // Gets the number of vertices in this mesh
        unsigned getNumberOfVertices() const { return m_numVertices; }

        // Gets the number of faces in this mesh
        unsigned getNumberOfFaces() const { return m_numFaces; }

        // Gets all vertices in this mesh
        const Vertices& getVertices() const { return m_vertices; }

        // Gets all faces in this mesh
        const Faces& getConnectivity() const { return m_faces; }

        // Returns a vertex at the specified index
        const Vertex& getVertex(const unsigned vertexIndex) const;

        // Returns a face at the specified index
        const Face& getFace(const unsigned faceIndex) const;

        // Gets the bounds of the mesh
        const common::Bounds& getBounds() const;

        // Gets the centroid of the mesh
        Vertex getCentroid() const;

        // Gets vertices
        VertexData getVertexData() const;

        // Gets connectivity data in the form a pointer to a contiguous memory
        void getConnectivityData(size_t& numBytes, unsigned*& connData) const;

        // Transforms a copy of this mesh and returns the copy
        std::unique_ptr<Mesh> transform(const glm::mat4& transformMatrix) const;

        // Writes this mesh to a STL file
        void writeToSTL(const std::string& stlFile) const;

        // Gets normals
        NormalData getNormals(const common::NormalLocation) const;

    protected:
        void generateRenderData() override;
        void generateColors() override;

    private:
        unsigned m_numVertices;
        unsigned m_numFaces;
        Vertices m_vertices;
        Faces m_faces;
        std::optional<common::Bounds> m_bounds;
        std::optional<Octree> m_octree;
        std::unique_ptr<unsigned> m_connectivity;
        size_t m_connectivityDataSize;
        std::optional<NormalData> m_vertexNormals;
        std::optional<NormalData> m_faceNormals;
        std::optional<VertexData> m_vertexData;

    private:
        void buildConnectivityData();
        void buildBounds();
        void generateVertexNormals();
        void generateFaceNormals();
        void buildVertexData();

};

}

#endif
