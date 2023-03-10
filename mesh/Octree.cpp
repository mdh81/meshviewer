#include "Octree.h"
#include "Mesh.h"
#include "Util.h"
#include <algorithm>
#include <numeric>
#include <iostream>
using namespace std;

namespace mv {

using namespace common;

// Initialize static variable
Octree* Octree::Octant::sm_octree = nullptr;

Octree::Octree(Mesh& mesh, const unsigned maxVerticesInOctant) :
    m_mesh(mesh),
    m_maxVerticesInOctant(maxVerticesInOctant),
    m_depth(1) {
    
    // All octants share a pointer to this parent octree
    // to keep per octant overhead to a minimum
    Octree::Octant::sm_octree = this;

    // NOTE: Create the root octant after setting the static member
    // variable in Octant. The constructor of Octant relies on
    // this static variable being set so it can get hold of
    // the mesh reference from the static pointer variable
    m_root = Octree::Octant(nullptr, {}, 0);

    // Build the octree by recursively subdividing octants
    m_root.subdivide();
}

void Octree::getNeighboringVertices(const unsigned vertexIndex, common::VertexIndices& neighbors) {
    
    // Find the leaf octant containing the vertex and copy all
    // vertices in that leaf octant into the result
    Octree::Octant::LeafOctants leafOctants;
    getLeafOctants(m_root, leafOctants); 
    neighbors.clear();
    neighbors.reserve(leafOctants.size() * m_maxVerticesInOctant);
    for (auto leafOctant : leafOctants) {
        if (leafOctant.get().hasVertex(vertexIndex)) {
            copy(leafOctant.get().m_vertices.begin(), leafOctant.get().m_vertices.end(), std::back_inserter(neighbors));
            break;
        }
    }
}


bool Octree::Octant::hasVertex(const unsigned vertexIndex) const {
    const Vertex& vertex = getMesh().getVertex(vertexIndex); 
    return 
        Util::isGreaterOrEqual(vertex.x, m_bounds.xmin) && 
        Util::isLessOrEqual(vertex.x, m_bounds.xmax) &&
        Util::isGreaterOrEqual(vertex.y, m_bounds.ymin) && 
        Util::isLessOrEqual(vertex.y, m_bounds.ymax) &&
        Util::isGreaterOrEqual(vertex.z, m_bounds.zmin) && 
        Util::isLessOrEqual(vertex.z, m_bounds.zmax);
}

Octree::Octant::LeafOctants Octree::getLeafOctants() {
    Octree::Octant::LeafOctants leaves;
    getLeafOctants(m_root, leaves);
    return leaves;
}

void Octree::getLeafOctants(Octree::Octant& octant, Octree::Octant::LeafOctants& leafOctants) {
    // An octant is a leaf if it doesn't have any children. We further restrict the definition
    // by including only those leaves that have at least one vertex in them
    bool hasChildren = std::any_of(octant.m_childOctants.begin(), octant.m_childOctants.end(),
                                   [](auto& childOctant) { return childOctant != nullptr; });
    if (!hasChildren && !octant.m_vertices.empty()) {
        leafOctants.push_back(std::ref(octant));
        return;
    } else {
        for(auto& childOctant : octant.m_childOctants) {
            if (childOctant) {
                getLeafOctants(*childOctant.get(), leafOctants);
            } 
        }
    }
}

Octree::Octant::Octant(Octant* parent, const Bounds& bounds, unsigned level) :
    m_level(level) {
    
    if (parent) {
        if (sm_octree->isDebugOn()) cerr << "Creating child octant at level " << m_level << " with bounds " << bounds;
        
        // Set bounds
        m_bounds = bounds;
        // Initialize this octant with vertices that are inside this octant
        populate(parent->m_vertices); 
    } else {
        // Since this is the root octant its bounds should be that of the mesh
        m_bounds = getMesh().getBounds();
        // All vertices in the mesh belong in the root octant
        m_vertices.resize(getMesh().getVertices().size());
        iota(m_vertices.begin(), m_vertices.end(), 0); 
        
        if (sm_octree->isDebugOn()) cerr << "Creating root octant with bounds " << m_bounds;
    }
}

void Octree::Octant::populate(const VertexIndices& parentVertices) {
    
    // Allocate enough memory for maximum possible vertex list
    m_vertices.reserve(parentVertices.size());

    // Copy vertices in the parent that reside inside this child octant
    // NOTE: uses back_inserter to be able to push_back into m_vertices
    copy_if(parentVertices.begin(), parentVertices.end(), 
            std::back_inserter(m_vertices), [&parentVertices, this](auto vertIndex) {
                
                // Skip vertices that were previously added to other octants
                if (sm_octree->m_levelToVerticesMap[m_level].find(vertIndex) != 
                    sm_octree->m_levelToVerticesMap[m_level].end()) {
                        if (sm_octree->isDebugOn()) 
                            cerr << "Skipping vertex " << vertIndex << " that was added to level " << m_level << " already" << endl;
                        return false;
                }
                
                // Check vertex against octant bounds
                bool vertexInOctant = hasVertex(vertIndex); 
                
                // Flag vertices that are within any octant in the current tree level 
                if (vertexInOctant) {

                    sm_octree->m_levelToVerticesMap[m_level].insert(vertIndex);
                    
                    if (sm_octree->isDebugOn())
                        cerr << "Adding vertex (index = " << vertIndex << " coordinates = "
                             << sm_octree->m_mesh.getVertex(vertIndex) << ") to octant" << endl;
                }
                
                return vertexInOctant;
        });
    // Release extra memory
    m_vertices.shrink_to_fit();
}

void Octree::Octant::subdivide() {

    if (sm_octree->isDebugOn()) {
        cerr << "Subdividing Octant at level " << m_level << endl;
    }

    // Terminte subdivision if the termination criteria is met 
    if (m_vertices.size() <= sm_octree->m_maxVerticesInOctant) { 
        if (sm_octree->isDebugOn()) cerr << "Number of vertices in octant is " << m_vertices.size() << ". Termination criteria met." << endl;
        return;
    }
    
    // Increment tree depth
    ++sm_octree->m_depth;

    // Split this octant into 8 child octants
    for (auto octantId : OctantIdIterator()) {
        m_childOctants[static_cast<int>(octantId)].reset(new Octree::Octant(this, getChildOctantBounds(octantId), m_level+1));
    }

    // Once the vertex information is carried over from the parent to the children, it's time to remove
    // them from the parent. Only the leaf octants should have vertices associated with them 
    this->m_vertices.clear();

    // Subdivide recursively
    for (auto& childOctant : m_childOctants) {
        childOctant->subdivide();
    }
}

Bounds Octree::Octant::getChildOctantBounds(const OctantId childId) {
    switch (childId) {
        case OctantId::Bottom_Left_Back:
            return { m_bounds.xmin, m_bounds.ymin, m_bounds.zmin, m_bounds.xmid(), m_bounds.ymid(), m_bounds.zmid() };
        case OctantId::Bottom_Right_Back:
            return { m_bounds.xmid(), m_bounds.ymin, m_bounds.zmin, m_bounds.xmax, m_bounds.ymid(), m_bounds.zmid() };
        case OctantId::Top_Left_Back:
            return { m_bounds.xmin, m_bounds.ymid(), m_bounds.zmin, m_bounds.xmid(), m_bounds.ymax, m_bounds.zmid() };
        case OctantId::Top_Right_Back:
            return { m_bounds.xmid(), m_bounds.ymid(), m_bounds.zmin, m_bounds.xmax, m_bounds.ymax, m_bounds.zmid() };
        case OctantId::Bottom_Left_Front:
            return { m_bounds.xmin, m_bounds.ymin, m_bounds.zmid(), m_bounds.xmid(), m_bounds.ymid(), m_bounds.zmax };
        case OctantId::Bottom_Right_Front:
            return { m_bounds.xmid(), m_bounds.ymin, m_bounds.zmid(), m_bounds.xmax, m_bounds.ymid(), m_bounds.zmax };
        case OctantId::Top_Left_Front:
            return { m_bounds.xmin, m_bounds.ymid(), m_bounds.zmid(), m_bounds.xmid(), m_bounds.ymax, m_bounds.zmax };
        case OctantId::Top_Right_Front:
            return { m_bounds.xmid(), m_bounds.ymid(), m_bounds.zmid(), m_bounds.xmax, m_bounds.ymax, m_bounds.zmax };
        default:
            throw std::runtime_error("Unknown octant id"); 
    }
}

}
