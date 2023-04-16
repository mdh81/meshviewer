#include "gtest/gtest.h"
#include "ReaderFactory.h"
#include "Mesh.h"
#include <memory>
#include <filesystem>
using namespace std;
using namespace mv;
using namespace mv::common;
using namespace mv::readers;

class STLReaderFixture : public ::testing::Test {
    protected:
        void SetUp() override {
            auto* pData = getenv("modelsDir");
            if (!pData) throw std::runtime_error("modelsDir environment variable not set");        
            m_modelsDir = pData; 
        }
        filesystem::path m_modelsDir;
};

TEST_F(STLReaderFixture, Read) {
    ReaderPointer stlReader = ReaderFactory::getReader(m_modelsDir/"cube.stl");
    auto spMesh = stlReader->getOutput();
    ASSERT_EQ(spMesh->getNumberOfVertices(), 36) << "Incorrect number of vertices" << std::endl;
    ASSERT_EQ(spMesh->getNumberOfFaces(), 12) << "Incorrect number of faces" << std::endl;
    auto& v = spMesh->getVertex(0);
    ASSERT_TRUE((v.x-1.0) < 1e-6 && (v.y-1.0) < 1e-6 && (v.z-1.0) < 1e-6) << "Vertex 0 is wrong" << std::endl;
    auto& v1 = spMesh->getVertex(35);
    ASSERT_TRUE((v1.x-1.0) < 1e-6 && (v1.y-1.0) < 1e-6 && (v1.z+1.0) < 1e-6) << "Vertex 0 is wrong" << std::endl;
    auto& f = spMesh->getFace(0);
    ASSERT_TRUE(f[0] == 0 && f[1] == 1 && f[2] == 2) << "Face 0 is wrong" << std::endl;
    auto& f1 = spMesh->getFace(11);
    ASSERT_TRUE(f1[0] == 33 && f1[1] == 34 && f1[2] == 35) << "Face 0 is wrong" << std::endl;
}

TEST_F(STLReaderFixture, Cleanup) {
    ReaderPointer stlReader = ReaderFactory::getReader(m_modelsDir/"cube.stl");
    auto spMesh = stlReader->getOutput(true);
    ASSERT_EQ(spMesh->getNumberOfVertices(), 8) << "Incorrect number of vertices" << std::endl;
}
