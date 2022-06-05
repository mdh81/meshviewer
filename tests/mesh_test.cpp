#include "gtest/gtest.h"
#include "Mesh.h"
#include "STLReader.h"
#include <vector>
#include <initializer_list>
using namespace std;
using namespace meshviewer;

TEST(Mesh, TestAddVertex) {
    Mesh m;
    m.initialize(10, 10);
    m.addVertex(1,2,3);
    m.addVertex(2,3,4);
    m.addFace({1,2,3});
}

TEST(Mesh, RemoveDuplicateVertices) {
    unique_ptr<Mesh> spMesh;
    STLReader("./cube.stl").getOutput(spMesh);
    int numOrigVertices = spMesh->getNumberOfVertices();
    spMesh->removeDuplicateVertices();
    int numNewVertices = spMesh->getNumberOfVertices();
    ASSERT_NE(numOrigVertices, numNewVertices);
    ASSERT_EQ(numNewVertices, 12);
}

TEST(Mesh, TestCentroid) {
    Mesh m;
    m.initialize(2, 1);
    m.addVertex(-5, 0, 0);
    m.addVertex(+5, 0, 0);
    common::Vertex centroid = m.getCentroid();
    ASSERT_FLOAT_EQ(centroid.x, 0.0) << "Centroid X coordinate is incorrect" << endl;
    ASSERT_FLOAT_EQ(centroid.y, 0.0) << "Centroid Y coordinate is incorrect" << endl;
    ASSERT_FLOAT_EQ(centroid.z, 0.0) << "Centroid Z coordinate is incorrect" << endl;
}
