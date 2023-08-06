#include "gtest/gtest.h"
#include "Octree.h"
#include "ReaderFactory.h"
#include "Types.h"
#include <vector>
#include <numeric>
#include <algorithm>
#include <filesystem>
using namespace std;
using namespace mv;
using namespace mv::common;
using namespace mv::readers;

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
    auto spMesh = ReaderFactory::getReader(m_modelsDir/"cube.stl")->getOutput();
    Octree octree(*spMesh.get());
    // Assert the root octant's bounds match the mesh's bounds
    auto meshBounds = spMesh->getBounds();
    auto& root = octree.getRoot();
    auto& rootBounds = octree.getBounds(octree.getRoot());
    ASSERT_NEAR(rootBounds.x.min, meshBounds.x.min, 1e-6);
    ASSERT_NEAR(rootBounds.y.min, meshBounds.y.min, 1e-6);
    ASSERT_NEAR(rootBounds.z.min, meshBounds.z.min, 1e-6);

    ASSERT_NEAR(rootBounds.x.max, meshBounds.x.max, 1e-6);
    ASSERT_NEAR(rootBounds.y.max, meshBounds.y.max, 1e-6);
    ASSERT_NEAR(rootBounds.z.max, meshBounds.z.max, 1e-6);

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
    auto spMesh = ReaderFactory::getReader(m_modelsDir/"cube.stl")->getOutput();
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
        childBoundsUnion.x.min = min(octree1.getBounds(*child).x.min, childBoundsUnion.x.min);
        childBoundsUnion.y.min = min(octree1.getBounds(*child).y.min, childBoundsUnion.y.min);
        childBoundsUnion.z.min = min(octree1.getBounds(*child).z.min, childBoundsUnion.z.min);

        childBoundsUnion.x.max = max(octree1.getBounds(*child).x.max, childBoundsUnion.x.max);
        childBoundsUnion.y.max = max(octree1.getBounds(*child).y.max, childBoundsUnion.y.max);
        childBoundsUnion.z.max = max(octree1.getBounds(*child).z.max, childBoundsUnion.z.max);
    }
    ASSERT_NEAR(childBoundsUnion.x.min, parentBounds.x.min, 1e-6);
    ASSERT_NEAR(childBoundsUnion.y.min, parentBounds.y.min, 1e-6);
    ASSERT_NEAR(childBoundsUnion.z.min, parentBounds.z.min, 1e-6);

    ASSERT_NEAR(childBoundsUnion.x.max, parentBounds.x.max, 1e-6);
    ASSERT_NEAR(childBoundsUnion.y.max, parentBounds.y.max, 1e-6);
    ASSERT_NEAR(childBoundsUnion.z.max, parentBounds.z.max, 1e-6);

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
    auto spMesh = ReaderFactory::getReader(m_modelsDir/"cube.stl")->getOutput();

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
