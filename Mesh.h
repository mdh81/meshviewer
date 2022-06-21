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
#include "glm/glm.hpp"

namespace meshviewer {

class Mesh : public MeshViewerObject {
    public:
        Mesh() = default;

        ~Mesh() = default;

        Mesh(const Mesh &);

        // TODO: Define copy assignment
        //       Define move assignment
        //       Define move constructor


    public:
        // TODO: Allow creation of meshes without this call. Make addVertex
        // and addFace allocate memory as needed. Come up with resonable
        // default initial allocation
        void initialize(const unsigned numVerts, const unsigned numFaces);
        
        // Adds a new vertex to this mesh
        void addVertex(const float x, const float y, const float z);
        
        // Adds a new face that is described the specified vertex identifiers
        // NOTE: vertexIds are 0-based 
        void addFace(const std::initializer_list<unsigned>& vertexIds);
        
        // Merges coincident vertices and adjusts the connectivity data
        // accordingly
        void removeDuplicateVertices();
        
        // Gets the number of vertices in this mesh
        unsigned getNumberOfVertices() const { return m_numVertices; }
        
        // Gets the number of faces in this mesh
        unsigned getNumberOfFaces() const { return m_numFaces; }
    
        // Gets all vertices in this mesh
        const common::Vertices& getVertices() const { return m_vertices; }
        
        // Gets all faces in this mesh
        const common::Faces& getConnectivity() const { return m_faces; }
        
        // Returns a vertex at the specified index
        const common::Vertex& getVertex(unsigned vertexIndex) const;

        // Returns a face at the specified index 
        const common::Face& getFace(unsigned faceIndex) const;
        
        // Gets the bounds of the mesh
        const common::Bounds& getBounds() const;
        
        // Gets the centroid of the mesh
        common::Vertex getCentroid() const;
        
        // Gets vertex data in the form of a pointer to a contiguous memory
        void getVertexData(size_t& numBytes, common::Vertex*& vertexData) const;
        
        // Gets connectivity data in the form a pointer to a contiguous memory 
        void getConnectivityData(size_t& numBytes, unsigned*& connData) const;
        
        // Transforms a copy of this mesh and returns the copy
        std::unique_ptr<Mesh> transform(const glm::mat4& transformMatrix) const; 
        
        // Writes this mesh to a STL file
        void writeToSTL(const std::string& stlFile) const;

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
