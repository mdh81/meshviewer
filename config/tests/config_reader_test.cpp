#include "gtest/gtest.h"
#include "ConfigReader.h"
#include <memory>
#include <fstream>
using namespace std;
using namespace mv::config;

class ConfigReaderFixture : public ::testing::Test {
    void SetUp() override {
        ofstream ofs("./test.cfg");
        ofs << "varA=valA" << endl;
        ofs << "varB=valB" << endl;
        ofs << "varC=valC" << endl;
        ofs << "boolVar=false" << endl;
        ofs.close();
    }
};

namespace mv { namespace config {
    class ConfigReaderTest {
    public:
        static ConfigReader &getReader() {
            static ConfigReader cfgReader("./test.cfg");
            return cfgReader;
        }
    };
} }


TEST_F(ConfigReaderFixture, ReadConfig) {
    ConfigReader& cr = ConfigReaderTest::getReader();
    ASSERT_EQ(cr.getValue("varA"), "valA") << "Config map has unexpected state" << endl;
    ASSERT_FALSE(cr.getBoolean("boolVar")) << "Config map has unexpected state" << endl;
}

TEST_F(ConfigReaderFixture, InvalidConfigFile) {

}

TEST_F(ConfigReaderFixture, InvalidConfigEntry) {
    ConfigReader& cr = ConfigReaderTest::getReader();
    EXPECT_THROW({
        try {
            cr.getBoolean("boolVarThatDoesntExist");
        } catch (std::runtime_error& ex) {
            throw;
        }
    }, std::runtime_error) << "Expected an exception for invalid keys!" << endl;
}
