#include "gtest/gtest.h"
#include "ReaderFactory.h"
#include <memory>
#include <fstream>
using namespace std;
using namespace mv;
using namespace mv::readers;

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
            ReaderFactory::getReader("dummy");
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
            ReaderFactory::getReader("dummy.nonexistent");
        } catch (std::runtime_error& ex) {
            throw;
        }
    }, std::runtime_error
    ) << "No error raised when file without extension is specified";
}

TEST_F(ReaderFixture, UnsupportedFormat) {
    EXPECT_THROW(
    {
        try {
            ReaderFactory::getReader((m_modelsDir/"dummy.unsupported").string());
        } catch (std::runtime_error& ex) {
            throw;
        }
    }, std::runtime_error
    ) << "No error raised when file without extension is specified";
}

TEST_F(ReaderFixture, SupportedFormats) {
    EXPECT_NO_THROW(
    {
        try {
            ReaderFactory::getReader((m_modelsDir/"cube.stl").string());
        } catch (std::runtime_error& ex) {
            throw;
        }
    }
    ) << "No error raised when file without extension is specified";
}