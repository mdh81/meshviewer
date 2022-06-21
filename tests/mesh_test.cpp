#include "gtest/gtest.h"
#include "Mesh.h"
#include "STLReader.h"
#include "glm/glm.hpp"
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

TEST(Mesh, TestConnectivityData) {
    Mesh m;
    m.initialize(4, 2);
    m.addVertex(-5, -5, 0);
    m.addVertex(+5, -5, 0);
    m.addVertex(+5, +5, 0);
    m.addVertex(-5, +5, 0);
    m.addFace({0, 1, 3});
    m.addFace({1, 2, 3});
    size_t numBytes = 0;
    unsigned* connData; 
    m.getConnectivityData(numBytes, connData);
    ASSERT_EQ(24, numBytes);
    ASSERT_EQ(0, connData[0]);
    ASSERT_EQ(1, connData[1]);
    ASSERT_EQ(3, connData[2]);

    ASSERT_EQ(1, connData[3]);
    ASSERT_EQ(2, connData[4]);
    ASSERT_EQ(3, connData[5]);
}

TEST(Mesh, TransformMesh) {
    Mesh m;
    m.initialize(2, 1);
    m.addVertex(0, 0, 0);
    m.addVertex(5, 0, 0);
    m.addFace({0,1});
    glm::mat4 translate(1.0f);
    translate[3] = glm::vec4(10, 10, 10, 1.0);
    std::unique_ptr<Mesh> m1 (std::move(m.transform(translate)));
    auto v = m1->getVertex(0);
    ASSERT_FLOAT_EQ(v.x, 10);
    ASSERT_FLOAT_EQ(v.y, 10);
    ASSERT_FLOAT_EQ(v.z, 10);
    v = m1->getVertex(1);
    ASSERT_FLOAT_EQ(v.x, 15);
    ASSERT_FLOAT_EQ(v.y, 10);
    ASSERT_FLOAT_EQ(v.z, 10);
    auto faces1 = m1->getConnectivity();
    ASSERT_EQ(faces1.at(0).at(0), 0);
    ASSERT_EQ(faces1.at(0).at(1), 1);

}

TEST(Mesh, WriteSTL) {
    unique_ptr<Mesh> spMesh;
    STLReader("./cube.stl").getOutput(spMesh);
    spMesh->writeToSTL("./cubeOut.stl");

    // TODO: Do a stat call and assert
    // a) cubeOut.stl exists
    // b) cubeOut.stl is same size as cube.stl
    // c) cubeOut.stl was written at the time the test was run
}
