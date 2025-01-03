#include "gtest/gtest.h"
#include "MockMeshFactory.h"
#include "ReaderFactory.h"
#include <memory>
#include <fstream>
using namespace std;
using namespace mv;
using namespace mv::readers;

#define MOCK_MESH_FACTORY std::make_unique<MockMeshFactory>()

class ReaderFixture : public ::testing::Test {
protected:
    void SetUp() override {
        auto* pData = getenv("modelsDir");
        if (!pData) throw std::runtime_error("modelsDir environment variable not set");
        m_modelsDir = pData;
        ofstream ofs ((m_modelsDir/"dummy.unsupported").string());
        ofs.close();
    }
    filesystem::path m_modelsDir;
};

TEST_F(ReaderFixture, FileWithoutExtension) {
    EXPECT_THROW(
    {
        try {
            ReaderFactory{MOCK_MESH_FACTORY}.getReader("dummy");
        } catch (std::runtime_error& ex) {
            throw;
        }
    }, std::runtime_error
    ) << "No error raised when file without extension is specified";
}

TEST_F(ReaderFixture, NonExistentFile) {
    EXPECT_THROW(
    {
        try {
            ReaderFactory{MOCK_MESH_FACTORY}.getReader("dummy.nonexistent");
        } catch (std::runtime_error& ex) {
            throw;
        }
    }, std::runtime_error
    ) << "No error raised when a non-existent file is specified";
}

TEST_F(ReaderFixture, UnsupportedFormat) {
    EXPECT_THROW(
    {
        try {
            ReaderFactory{MOCK_MESH_FACTORY}.getReader((m_modelsDir/"dummy.unsupported").string());
        } catch (std::runtime_error& ex) {
            throw;
        }
    }, std::runtime_error
    ) << "No error raised when unsupported file format is specified";
}

TEST_F(ReaderFixture, SupportedFormats) {
    EXPECT_NO_THROW(
    {
        try {
            ReaderFactory{MOCK_MESH_FACTORY}.getReader((m_modelsDir/"cube.stl").string());
            ReaderFactory{MOCK_MESH_FACTORY}.getReader((m_modelsDir/"Armadillo.ply").string());
        } catch (std::runtime_error& ex) {
            throw;
        }
    }
    ) << "Error unexpectedly raised when supported file formats were specified";
}

TEST(ReaderFactory, ReturnsValidLookupResultsWithSupportedExtensions) {
    ofstream {"a.stl"}; ofstream {"a.ply"}; ofstream {"a.sTl"}; ofstream {"a.PlY"}; ofstream {"a.ObJ"};
    ASSERT_TRUE(ReaderFactory{MOCK_MESH_FACTORY}.isFileTypeSupported("a.stl")) << "stl should be a supported type";
    ASSERT_TRUE(ReaderFactory{MOCK_MESH_FACTORY}.isFileTypeSupported("a.ply")) << "ply should be a supported type";
    ASSERT_TRUE(ReaderFactory{MOCK_MESH_FACTORY}.isFileTypeSupported("a.sTl")) << "stl should be a supported type";
    ASSERT_TRUE(ReaderFactory{MOCK_MESH_FACTORY}.isFileTypeSupported("a.PlY")) << "stl should be a supported type";
    ASSERT_FALSE(ReaderFactory{MOCK_MESH_FACTORY}.isFileTypeSupported("a.ObJ")) << "obj should be an unsupported type";
    ASSERT_FALSE(ReaderFactory{MOCK_MESH_FACTORY}.isFileTypeSupported(".")) << "directories should be an unsupported type";
    ASSERT_FALSE(ReaderFactory{MOCK_MESH_FACTORY}.isFileTypeSupported("abc.stl")) << "non-existent files should be "
                                                                                     "classified as unsupported";
}