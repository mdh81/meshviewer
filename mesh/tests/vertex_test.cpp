#include "gtest/gtest.h"
#include "MeshImpl.h"
#include "Face.h"
#include "3dmath/Vector.h"
using namespace std;
using namespace mv;

TEST(Vertex, FaceAssociation) {
    MeshImpl m;
    m.initialize(2, 1);
    m.addVertex(0, 0, 0);
    m.addVertex(5, 0, 0);
    m.addFace({0,1});
    vector<unsigned> faces;
    m.getVertex(0).getFaces(faces);
    ASSERT_EQ(faces.size(), 1);
    ASSERT_EQ(faces.at(0), 0);
    m.getVertex(1).getFaces(faces);
    ASSERT_EQ(faces.size(), 1);
    ASSERT_EQ(faces.at(0), 0);
}

TEST(Vertex, Normals) {
    MeshImpl m;
    m.initialize(4, 2);
    m.addVertex(5, 0, 0);
    m.addVertex(5, 5, 0);
    m.addVertex(-5, 5, 0);
    m.addVertex(5, 5, -5);
    m.addFace({0,1,2});
    m.addFace({0,1,3});
    
    // Normals at vertices that are not shared by faces should
    // be equal to face normals 
    auto vnormal1 = m.getVertex(2).getNormal(m);
    auto fnormal1 = m.getFace(0).getNormal(m);
    ASSERT_FLOAT_EQ(vnormal1.dot(fnormal1), 1); 
    
    auto vnormal2 = m.getVertex(3).getNormal(m);
    auto fnormal2 = m.getFace(1).getNormal(m);
    ASSERT_FLOAT_EQ(vnormal2.dot(fnormal2), 1); 

    // Normals at shared vertices should be average of the normals
    // of the face
    math3d::Vector<float, 3> avgNormal(
            {(fnormal1[0]+fnormal2[0])*0.5f,
             (fnormal1[1]+fnormal2[1])*0.5f,
             (fnormal1[2]+fnormal2[2])*0.5f});
    avgNormal.normalize();
    
    // Both normals should be the same 
    auto vnormal3 = m.getVertex(0).getNormal(m);
    auto vnormal4 = m.getVertex(1).getNormal(m);
    ASSERT_FLOAT_EQ(vnormal3.dot(vnormal4), 1);

    // It should be the average
    ASSERT_FLOAT_EQ(vnormal3.dot(avgNormal), 1);

}
