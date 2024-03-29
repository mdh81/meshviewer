#pragma once
#include <memory>
#include <initializer_list>
#include <array>
#include <vector>
#include <optional>
#include <filesystem>
#include "Types.h"
#include "Drawable3D.h"
#include "Octree.h"
#include "Vertex.h"
#include "Face.h"

namespace mv {

class Mesh : public Drawable3D {
    public:
        using Vertices = std::vector<Vertex>;
        using Faces = std::vector<Face>;
        using NormalData = common::Array<float, 3>;
        using VertexData = common::Array<float, 3>;
        using MeshPointer = std::unique_ptr<mv::Mesh>;
        using Meshes = std::vector<MeshPointer>;

    public:
        Mesh();

        ~Mesh() override = default;

        Mesh(const Mesh &);

        // TODO: Define copy assignment
        //       Define move assignment
        //       Define move constructor
        //

    public:
        void render() override;

        [[nodiscard]]
        bool supportsGlyphs() const override { return true; }

    public:
        // TODO: Allow creation of meshes without this call. Make addVertex
        // and addFace allocate memory as needed. Come up with resonable
        // default initial allocation
        void initialize(unsigned numVertices, unsigned numFaces);

        // Adds a new vertex to this mesh
        void addVertex(float x, float y, float z);

        // Adds a new face that is described the specified vertex identifiers
        // NOTE: vertexIds are 0-based
        void addFace(const std::initializer_list<unsigned>& vertexIds);

        // Merges coincident vertices and adjusts the connectivity data
        // accordingly. Return number of duplicates that were removed
        unsigned removeDuplicateVertices();

        // Gets the number of vertices in this mesh
        [[nodiscard]] unsigned getNumberOfVertices() const { return m_numVertices; }

        // Gets the number of faces in this mesh
        [[nodiscard]] unsigned getNumberOfFaces() const { return m_numFaces; }

        // Gets all vertices in this mesh
        [[nodiscard]] const Vertices& getVertices() const { return m_vertices; }

        // Gets all faces in this mesh
        [[nodiscard]] const Faces& getConnectivity() const { return m_faces; }

        // Returns a vertex at the specified index
        [[nodiscard]] const Vertex& getVertex(unsigned vertexIndex) const;

        // Returns a face at the specified index
        [[nodiscard]] const Face& getFace(unsigned faceIndex) const;

        // Gets the bounds of the mesh
        [[nodiscard]] common::Bounds getBounds() const override;

        // Gets the centroid of the mesh
        [[nodiscard]] common::Point3D getCentroid() const override;

        // Gets vertices
        [[nodiscard]] VertexData getVertexData() const;

        // Gets connectivity data in the form a pointer to a contiguous memory
        void getConnectivityData(size_t& numBytes, unsigned*& connData) const;

        // Transforms a copy of this mesh and returns the copy
        [[nodiscard]] std::unique_ptr<Mesh> transform(common::TransformMatrix const& transformMatrix) const;

        // Writes this mesh to a STL file
        void writeToSTL(const std::string& stlFile) const;

        // Gets normals
        [[nodiscard]] NormalData getNormals(common::NormalLocation) const;

        void writeToFile(std::string const& fileName, common::TransformMatrix const& transform) const override {
            std::filesystem::path filePath = fileName;
            if (filePath.extension() == ".STL" || filePath.extension() == ".stl") {
                this->transform(transform)->writeToSTL(fileName);
            } else {
                throw std::runtime_error("Currently, only STL output for meshes is supported.");
            }
        }

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