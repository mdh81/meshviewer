#ifndef OCTREE_H
#define OCTREE_H
#include "Types.h"
#include "MeshViewerObject.h"
#include <array>
#include <exception>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace meshviewer {

class Mesh;

class Octree : public MeshViewerObject {
    public:
        Octree(Mesh& mesh, const unsigned maxVerticesInOctant = 100); 
        Mesh& getMesh() const { return m_mesh; }
        unsigned char getDepth() const { return m_depth; }
        void getNeighboringVertices(const unsigned vertexIndex, common::VertexIndices& neighbors); 

    private:
        class Octant {
            // Dummy default constructor to set the correct initialization
            // order in Octree constructor
            Octant() = default;

            Octant(Octant* parent, const common::Bounds& bounds, unsigned level);
            
            // Reference to the mesh that this octree was built for
            // NOTE: The pointer is static because we don't want to
            // incur the extra overhead of a pointer for each Octant 
            static Octree* sm_octree;

            // Bounds of this octant
            common::Bounds m_bounds;

            // Enumeration holding child octant position identifiers
            enum class OctantId : unsigned char {
                Bottom_Left_Back,
                Bottom_Right_Back,
                Top_Left_Back,
                Top_Right_Back,
                Bottom_Left_Front,
                Bottom_Right_Front,
                Top_Left_Front,
                Top_Right_Front
            };
            
            // An iterator over the OctantId enumeration
            using OctantIdIterator = common::EnumIterator<OctantId, OctantId::Bottom_Left_Back, OctantId::Top_Right_Front>;

            // 8 child octants arranged in the order of enumeration OctantId
            using OctantPtr = std::unique_ptr<Octant>;
            using ChildOctants = std::array<OctantPtr, 8>;
            ChildOctants m_childOctants;

            // Alias for leaf octants
            using LeafOctants = std::vector<std::reference_wrapper<Octant>>;

            // Level of this octant within the Octree
            unsigned m_level;

            // List of mesh vertices (by vertex index in the mesh) inside
            // this octant
            common::VertexIndices m_vertices;

            void populate(const common::VertexIndices& parentVertices);
            void subdivide();
            common::Bounds getChildOctantBounds(const OctantId childId);

            static Mesh& getMesh() { 
                if (!sm_octree) throw std::runtime_error("Parent octree pointer is invalid!");
                return sm_octree->getMesh();
            }

            bool hasVertex(const unsigned vertexIndex) const;

            // Make the outer class a friend so, it can access data members of the inner class
            // The inner class members will be inaccessible to anyone using the outer class to
            // ensure the inner class' implementation details will not be relied upon by any
            // external code
            friend Octree;

            friend std::ostream& operator << (std::ostream& os, Octree::Octant& octant);
        };

        friend std::ostream& operator << (std::ostream& os, Octree::Octant& octant);

    public:
        // Access to inner class data members. This is mostly for regression tests. 
        const Octree::Octant& getRoot() const { return m_root; }
        const common::Bounds& getBounds(const Octree::Octant& octant) { return octant.m_bounds; }
        const common::VertexIndices& getVertices(const Octree::Octant& octant) { return octant.m_vertices; }
        const Octree::Octant::ChildOctants& getChildren(const Octree::Octant& octant) { return octant.m_childOctants; } 
        Octree::Octant::LeafOctants getLeafOctants();
    
    private:
        void getLeafOctants(Octree::Octant& octant, Octree::Octant::LeafOctants& leafOctants);

    private:
        Mesh& m_mesh;
        const unsigned m_maxVerticesInOctant;
        Octree::Octant m_root;
        using VertexIndexSet = std::unordered_set<unsigned>;
        using LevelToVerticesMap = std::unordered_map<unsigned, VertexIndexSet>; 
        LevelToVerticesMap m_levelToVerticesMap; 
        unsigned char m_depth;
};


inline std::ostream& operator << (std::ostream& os, Octree::Octant& octant) {
    os << std::endl;
    os << "Level: " << octant.m_level << std::endl;
    os << "Num Vertices: " << octant.m_vertices.size() << std::endl; 
    os << "Bounds " << octant.m_bounds << std::endl << std::endl;
    return os;
}

}

#endif
