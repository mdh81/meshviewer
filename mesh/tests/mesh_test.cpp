#include "gtest/gtest.h"
#include "Mesh.h"
#include "STLReader.h"
#include "glm/glm.hpp"
#include <vector>
#include <initializer_list>
#include <string>
#include <filesystem>
#include <cstdlib>
using namespace std;
using namespace meshviewer;

class MeshFixture : public ::testing::Test {
    protected:
        void SetUp() override {
            auto* pData = getenv("modelsDir");
            if (!pData) throw std::runtime_error("modelsDir environment variable not set");        
            m_modelsDir = pData; 
        }
    
        filesystem::path m_modelsDir;
};

TEST_F(MeshFixture, RemoveDuplicateVertices) {
    unique_ptr<Mesh> spMesh;
    
    STLReader(MeshFixture::m_modelsDir/"cube.stl").getOutput(spMesh);
    int numOrigVertices = spMesh->getNumberOfVertices();
    spMesh->removeDuplicateVertices();
    int numNewVertices = spMesh->getNumberOfVertices();
    ASSERT_NE(numOrigVertices, numNewVertices);
    ASSERT_EQ(numNewVertices, 8);
}

TEST(Mesh, TestCentroid) {
    Mesh m;
    m.initialize(2, 1);
    m.addVertex(-5, 0, 0);
    m.addVertex(+5, 0, 0);
    Vertex centroid = m.getCentroid();
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

TEST_F(MeshFixture, WriteSTL) {
    unique_ptr<Mesh> spMesh;
    auto inputPath = MeshFixture::m_modelsDir/"cube.stl";
    auto inputFs = filesystem::file_size(inputPath);
    ASSERT_TRUE(inputFs > 0) << "Input STL file is bad";
    STLReader(inputPath).getOutput(spMesh);
    auto outputPath = MeshFixture::m_modelsDir/"cubeOut.stl";
    filesystem::remove(outputPath);
    spMesh->writeToSTL(outputPath);
    auto outputFs = filesystem::file_size(outputPath);
    ASSERT_TRUE(inputFs == outputFs) << "Output STL was supposed to be identical to the input";
}


TEST(Mesh, Normals) {
    Mesh m;
    m.initialize(4, 2);
    m.addVertex(5, 0, 0);
    m.addVertex(5, 5, 0);
    m.addVertex(-5, 5, 0);
    m.addVertex(5, 5, -5);
    m.addFace({0,1,2});
    m.addFace({0,1,3});

    auto vnormals = m.getNormals(common::NormalLocation::Vertex);
    ASSERT_EQ(vnormals.getSize(), 4);
    ASSERT_EQ(vnormals.getDataSize(), 4 * 12);
    ASSERT_NE(vnormals.getData(), nullptr);
    // Vertex 3 has a normal of +z
    math3d::Vector<float, 3> v{vnormals.getData()[6], vnormals.getData()[7], vnormals.getData()[8]};
    ASSERT_FLOAT_EQ(v.dot(math3d::Vector<float,3>{0,0,1}), 1);
    // Vertex 3 has a normal of -x
    math3d::Vector<float, 3> v1{vnormals.getData()[9], vnormals.getData()[10], vnormals.getData()[11]};
    ASSERT_FLOAT_EQ(v1.dot(math3d::Vector<float,3>{-1,0,0}), 1);
    
    // Face 0 has a normal of +z and 1 has a normal of -x 
    auto fnormals = m.getNormals(common::NormalLocation::Face);
    ASSERT_EQ(fnormals.getSize(), 2);
    ASSERT_EQ(fnormals.getDataSize(), 2 * 12);
    ASSERT_NE(fnormals.getData(), nullptr);
    ASSERT_FLOAT_EQ((
                math3d::Vector<float,3>{fnormals.getData()[0], fnormals.getData()[1], fnormals.getData()[2]}.dot(
                        math3d::Vector<float,3>{0,0,1})), 1);
    ASSERT_FLOAT_EQ((
                math3d::Vector<float,3>{fnormals.getData()[3], fnormals.getData()[4], fnormals.getData()[5]}.dot(
                        math3d::Vector<float,3>{-1,0,0})), 1);

}
