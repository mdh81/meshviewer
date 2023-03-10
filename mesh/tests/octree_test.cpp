#include "gtest/gtest.h"
#include "Mesh.h"
#include "Octree.h"
#include "STLReader.h"
#include "Types.h"
#include <vector>
#include <numeric>
#include <algorithm>
#include <filesystem>
using namespace std;
using namespace mv;
using namespace mv::common;

class OctreeFixture : public ::testing::Test {
    protected:
        void SetUp() override {
            auto* pData = getenv("modelsDir");
            if (!pData) throw std::runtime_error("modelsDir environment variable not set");        
            m_modelsDir = pData; 
        }
    
        filesystem::path m_modelsDir;
};

TEST_F(OctreeFixture, RootOctant) {
    std::unique_ptr<Mesh> spMesh;
    STLReader reader(OctreeFixture::m_modelsDir/"cube.stl"); 
    reader.getOutput(spMesh);
    Octree octree(*spMesh.get());
    // Assert the root octant's bounds match the mesh's bounds
    auto& meshBounds = spMesh->getBounds();
    auto& root = octree.getRoot();
    auto& rootBounds = octree.getBounds(octree.getRoot());
    ASSERT_NEAR(rootBounds.xmin, meshBounds.xmin, 1e-6);
    ASSERT_NEAR(rootBounds.ymin, meshBounds.ymin, 1e-6);
    ASSERT_NEAR(rootBounds.zmin, meshBounds.zmin, 1e-6);

    ASSERT_NEAR(rootBounds.xmax, meshBounds.xmax, 1e-6);
    ASSERT_NEAR(rootBounds.ymax, meshBounds.ymax, 1e-6);
    ASSERT_NEAR(rootBounds.zmax, meshBounds.zmax, 1e-6);

    // Assert the root octact's vertices list has all the
    // vertices in the mesh
    ASSERT_EQ(octree.getVertices(octree.getRoot()).size(), spMesh->getNumberOfVertices());
    vector<unsigned> vec(spMesh->getNumberOfVertices());
    iota(vec.begin(), vec.end(), 0);
    for (size_t i = 0; i < vec.size(); ++i) {
        ASSERT_EQ(octree.getVertices(octree.getRoot()).at(i), vec.at(i));
    }
}

TEST_F(OctreeFixture, Subdivision) {
    std::unique_ptr<Mesh> spMesh;
    STLReader reader(OctreeFixture::m_modelsDir/"cube.stl");
    reader.getOutput(spMesh);
    // Create octree with 100 vertices per octant
    Octree octree(*spMesh.get(), 100);
    // The triangulated cube above will have 36 vertices, so only one 
    // octant should have been created 
    ASSERT_EQ(octree.getDepth(), 1);

    // Create octree with 10 vertices per octant
    Octree octree1(*spMesh.get(), 10);
    octree1.debugOn();
    // There should be more octants due to multiple subdivisions
    ASSERT_GE(octree1.getDepth(), 1);
    // Assert that child octants are in the correct position
    auto& rootOctant = octree1.getRoot();
    auto parentBounds = octree1.getBounds(rootOctant);
    auto& children = octree1.getChildren(rootOctant);
    Bounds childBoundsUnion;
    for (auto& child : children) {
        childBoundsUnion.xmin = min(octree1.getBounds(*child).xmin, childBoundsUnion.xmin);
        childBoundsUnion.ymin = min(octree1.getBounds(*child).ymin, childBoundsUnion.ymin);
        childBoundsUnion.zmin = min(octree1.getBounds(*child).zmin, childBoundsUnion.zmin);

        childBoundsUnion.xmax = max(octree1.getBounds(*child).xmax, childBoundsUnion.xmax);
        childBoundsUnion.ymax = max(octree1.getBounds(*child).ymax, childBoundsUnion.ymax);
        childBoundsUnion.zmax = max(octree1.getBounds(*child).zmax, childBoundsUnion.zmax);
    }
    ASSERT_NEAR(childBoundsUnion.xmin, parentBounds.xmin, 1e-6);
    ASSERT_NEAR(childBoundsUnion.ymin, parentBounds.ymin, 1e-6);
    ASSERT_NEAR(childBoundsUnion.zmin, parentBounds.zmin, 1e-6);

    ASSERT_NEAR(childBoundsUnion.xmax, parentBounds.xmax, 1e-6);
    ASSERT_NEAR(childBoundsUnion.ymax, parentBounds.ymax, 1e-6);
    ASSERT_NEAR(childBoundsUnion.zmax, parentBounds.zmax, 1e-6);

    // Assert that the subdivision criteria (10 vertices per leaf octant)
    auto leaves = octree1.getLeafOctants();
    ASSERT_TRUE(leaves.size() > 0);
    for (auto& leaf : leaves) {
        ASSERT_LE(octree1.getVertices(leaf.get()).size(), 10);
    }

    // Assert that all vertices in the mesh are present in the leaf octant
    unordered_map<unsigned, bool> vertIndexMap;
    for (auto& leaf : leaves) {
        mv::common::VertexIndices vi = octree1.getVertices(leaf.get());
        for (auto v : vi) {
            // Assert no duplicates are present either within a leaf octant
            // or across a leaf octants
            ASSERT_TRUE(vertIndexMap.find(v) == vertIndexMap.end());
            vertIndexMap[v] = true;
        }
    }
    for(unsigned i = 0; i < 36; ++i) {
        ASSERT_TRUE(vertIndexMap.find(i) != vertIndexMap.end());
        ASSERT_TRUE(vertIndexMap[i]);
    }

    // Assert that the non-leaf octants don't contain any vertices
    // In this case, the tree depth is 2, so ensure that the first
    // level's vertex list is empty
    ASSERT_TRUE(octree1.getVertices(octree1.getRoot()).empty());
}


TEST_F(OctreeFixture, NeighboringVertices) {
    std::unique_ptr<Mesh> spMesh;
    STLReader reader(OctreeFixture::m_modelsDir/"cube.stl");
    reader.getOutput(spMesh);
    
    // Create octree with 5 vertices per octant
    Octree octree(*spMesh.get(), 5);

    // Get all the neighboring vertices to the first vertex
    VertexIndices neighbors; 
    octree.getNeighboringVertices(0, neighbors); 
    
    // There should be 5 vertices in the octant neighborhood    
    ASSERT_EQ(neighbors.size(), 5);

    // Create octree with 100 vertices per octant
    Octree octree1(*spMesh.get(), 100);
    octree1.getNeighboringVertices(0, neighbors); 
    // There should be 36 vertices (original number of vertices in the mesh)
    // in the octant neighborhood since all vertices
    ASSERT_EQ(neighbors.size(), 36);

}
