#pragma once
#include <memory>
#include <initializer_list>
#include <array>
#include <vector>
#include <optional>
#include <filesystem>
#include "Types.h"
#include "Vertex.h"
#include "Face.h"
#include "Drawable3D.h"

namespace mv {

class Mesh : public virtual Drawable3D {
    public:
        using Vertices = std::vector<Vertex>;
        using Faces = std::vector<Face>;
        using NormalData = common::Array<float, 3>;
        using VertexData = common::Array<float, 3>;
        using MeshPointer = std::unique_ptr<mv::Mesh>;
        using Meshes = std::vector<MeshPointer>;

    public:
        // TODO: Allow creation of meshes without this call. Make addVertex
        // and addFace allocate memory as needed. Come up with resonable
        // default initial allocation
        virtual void initialize(unsigned numVertices, unsigned numFaces) = 0;

        // Adds a new vertex to this mesh
        virtual void addVertex(float x, float y, float z) = 0;

        // Adds a new face that is described with the specified vertex identifiers
        // NOTE: vertexIds are 0-based
        virtual void addFace(const std::initializer_list<unsigned>& vertexIds) = 0;

        // Merges coincident vertices and adjusts the connectivity data
        // accordingly. Return number of duplicates that were removed
        virtual unsigned removeDuplicateVertices() = 0;

        // Gets the number of vertices in this mesh
        virtual unsigned getNumberOfVertices() const  = 0;

        // Gets the number of faces in this mesh
        virtual unsigned getNumberOfFaces() const = 0;

        // Gets all vertices in this mesh
        virtual const Vertices& getVertices() const = 0;

        // Gets all faces in this mesh
        virtual const Faces& getConnectivity() const = 0;

        // Returns a vertex at the specified index
        virtual const Vertex& getVertex(unsigned vertexIndex) const = 0;

        // Returns a face at the specified index
        virtual const Face& getFace(unsigned faceIndex) const = 0;

        // Gets vertices
        virtual VertexData getVertexData() const = 0;

        // Gets connectivity data in the form a pointer to a contiguous memory
        virtual void getConnectivityData(size_t& numBytes, unsigned*& connData) const = 0;

        // Transforms a copy of this mesh and returns the copy
        virtual std::unique_ptr<Mesh> transform(common::TransformMatrix const& transformMatrix) const = 0;

        // Writes this mesh to a STL file
        virtual void writeToSTL(const std::string& stlFile) const = 0;

        // Gets normals
        virtual NormalData getNormals(common::NormalLocation) const = 0;

        virtual ~Mesh() = default;
};

}