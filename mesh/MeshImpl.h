#pragma once

#include "Mesh.h"
#include "Drawable3D.h"
#include "Octree.h"

namespace mv {

    class MeshImpl : public Mesh {

    public:
        MeshImpl();

        ~MeshImpl() = default;

        MeshImpl(MeshImpl const&);

        // TODO: Define copy assignment
        //       Define move assignment
        //       Define move constructor
        //

        void render() override;

        [[nodiscard]]
        bool supportsGlyphs() const override { return true; }

        void initialize(unsigned numVertices, unsigned numFaces) override;

        void addVertex(float x, float y, float z) override;

        void addFace(const std::initializer_list<unsigned> &vertexIds) override;

        [[nodiscard]]
        unsigned removeDuplicateVertices() override;

        // Gets the number of vertices in this mesh
        [[nodiscard]]
        unsigned getNumberOfVertices() const override;

        // Gets the number of faces in this mesh
        [[nodiscard]]
        unsigned getNumberOfFaces() const override;

        // Gets all vertices in this mesh
        [[nodiscard]]
        const Vertices &getVertices() const override;

        // Gets all faces in this mesh
        [[nodiscard]]
        const Faces &getConnectivity() const override;

        // Returns a vertex at the specified index
        [[nodiscard]]
        const Vertex &getVertex(unsigned vertexIndex) const override;

        // Returns a face at the specified index
        [[nodiscard]]
        const Face &getFace(unsigned faceIndex) const override;

        // Gets the bounds of the mesh
        [[nodiscard]]
        common::Bounds getBounds() const override;

        // Gets the centroid of the mesh
        [[nodiscard]]
        common::Point3D getCentroid() const override;

        // Gets vertices
        [[nodiscard]]
        VertexData getVertexData() const override;

        // Gets connectivity data in the form a pointer to a contiguous memory
        void getConnectivityData(size_t &numBytes, unsigned *&connData) const override;

        // Transforms a copy of this mesh and returns the copy
        [[nodiscard]]
        std::unique_ptr<Mesh> transform(common::TransformMatrix const &transformMatrix) const override;

        // Writes this mesh to a STL file
        void writeToSTL(const std::string &stlFile) const override;

        // Gets normals
        [[nodiscard]]
        NormalData getNormals(common::NormalLocation) const override;

        void writeToFile(std::string const &fileName, common::TransformMatrix const &transform) const override;

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