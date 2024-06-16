#include "gtest/gtest.h"
#include "STLReader.h"
#include "MockMeshFactory.h"
#include <memory>
#include <filesystem>
using namespace std;
using namespace mv;
using namespace mv::common;
using namespace mv::readers;
using namespace testing;

namespace mv::readers {

    class STLReaderFixture : public ::testing::Test {
    protected:
        void SetUp() override {
            auto *pData = getenv("modelsDir");
            if (!pData) throw std::runtime_error("modelsDir environment variable not set");
            m_modelsDir = pData;
        }

        void readData(std::string fileName) {
            cout << std::filesystem::current_path() << std::endl;
            ifstream ifs(m_modelsDir / fileName, ios::binary);
            ASSERT_TRUE(ifs) << "Unable to open cube.stl test file";
            stlReader.getOutput(ifs, mockMesh);
        }

        void readDataAndClean(std::string fileName) {
            ifstream ifs(m_modelsDir / fileName, ios::binary);
            ASSERT_TRUE(ifs) << "Unable to open cube.stl test file";
            stlReader.getOutput(ifs, mockMesh, /*clean=*/true);
        }

        filesystem::path m_modelsDir;
        mv::readers::STLReader stlReader{"", MockMeshFactory{}};
        std::unique_ptr<Mesh> mockMesh { new MockMesh{} };
    };

    TEST_F(STLReaderFixture, Read) {
        auto& mesh = dynamic_cast<MockMesh&>(*mockMesh.get());
        // 12 triangulated faces for a cube and 3 vertices per face since STL is not an indexed mesh
        EXPECT_CALL(mesh, initialize(12*3, 12)).Times(Exactly(1));
        // 36 vertices
        EXPECT_CALL(mesh, addVertex(_,_,_)).Times(Exactly(36));
        // 12 triangles
        EXPECT_CALL(mesh, addFace(_)).Times(Exactly(12));

        readData("cube.stl");
    }

    TEST_F(STLReaderFixture, Cleanup) {
        auto& mesh = dynamic_cast<MockMesh&>(*mockMesh.get());

        // Should call Mesh::removeDuplicateVertices when clean flag is on
        EXPECT_CALL(mesh, removeDuplicateVertices()).Times(Exactly(1));

        readDataAndClean("cube.stl");
    }
}