#include "gtest/gtest.h"
#include "Mesh.h"
#include "Face.h"
#include "3dmath/Vector.h"
using namespace std;
using namespace meshviewer;

TEST(Face, VertexAssociation) {
    Mesh m;
    m.initialize(2, 1);
    m.addVertex(0, 0, 0);
    m.addVertex(5, 0, 0);
    m.addFace({0,1});
    vector<unsigned> vertices;
    m.getFace(0).getVertices(vertices);
    ASSERT_EQ(vertices.size(), 2);
    ASSERT_EQ(vertices.at(0), 0);
    ASSERT_EQ(vertices.at(1), 1);
}

TEST(Face, Normals) {
    Mesh m;
    m.initialize(4, 2);
    m.addVertex(5, 0, 0);
    m.addVertex(5, 5, 0);
    m.addVertex(-5, 5, 0);
    m.addVertex(5, 5, -5);
    m.addFace({0,1,2});
    m.addFace({0,1,3});
    
    auto normal1 = m.getFace(0).getNormal(m);
    ASSERT_FLOAT_EQ(normal1[0], 0); 
    ASSERT_FLOAT_EQ(normal1[1], 0); 
    ASSERT_FLOAT_EQ(normal1[2], 1); 
    
    auto normal2 = m.getFace(1).getNormal(m);
    ASSERT_FLOAT_EQ(normal2[0], -1); 
    ASSERT_FLOAT_EQ(normal2[2],  0); 
    ASSERT_FLOAT_EQ(normal2[2],  0); 
    
}

TEST(Face, Centroid) {
    Mesh m;
    m.initialize(2, 1);
    m.addVertex(0, 0, 0);
    m.addVertex(5, 0, 0);
    m.addFace({0,1});
    auto c = m.getFace(0).getCentroid(m);
    ASSERT_FLOAT_EQ(2.5f, c.x);
    ASSERT_FLOAT_EQ(0.f, c.y);
    ASSERT_FLOAT_EQ(0.f, c.z);
}

