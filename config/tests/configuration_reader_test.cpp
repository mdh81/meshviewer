#include "gtest/gtest.h"
#include "ConfigurationReader.h"
#include "common/Types.h"
#include <memory>
#include <fstream>
using namespace std;
using namespace mv::config;
using namespace mv::common;

class ConfigurationReaderFixture : public ::testing::Test {
    void SetUp() override {
        ofstream ofs("./test.cfg");
        ofs << "varA=valA" << endl;
        ofs << "varB=valB" << endl;
        ofs << "varC=valC" << endl;
        ofs << "boolVar=false" << endl;
        ofs << "colorVar=255,10,25" << endl;
        ofs << "invalidColorVar1=255,10" << endl;
        ofs << "invalidColorVar2=255,,100" << endl;
        ofs << "#comment" << endl;
        ofs.close();
    }
};

namespace mv { namespace config {
    class ConfigurationReaderTest {
    public:
        static ConfigurationReader &getReader() {
            static ConfigurationReader cfgReader("./test.cfg");
            return cfgReader;
        }
        static unsigned getNumberOfKeysStartingWith(char c, ConfigurationReader& cfgReader) {
            unsigned result = 0;
            for (auto& key : cfgReader.data) {
                result += key.first.starts_with(c);
            }
            return result;
        }
    };
} }


TEST_F(ConfigurationReaderFixture, ReadStringConfiguration) {
    ConfigurationReader &cr = ConfigurationReaderTest::getReader();
    ASSERT_EQ(cr.getValue("varA"), "valA") << "Config map has unexpected state" << endl;
    ASSERT_EQ(cr.getValue("varB"), "valB") << "Config map has unexpected state" << endl;
    ASSERT_EQ(cr.getValue("varC"), "valC") << "Config map has unexpected state" << endl;
}

TEST_F(ConfigurationReaderFixture, ReadBooleanConfiguration) {
    ConfigurationReader &cr = ConfigurationReaderTest::getReader();
    ASSERT_FALSE(cr.getBoolean("boolVar")) << "Config map has unexpected state" << endl;
}

TEST_F(ConfigurationReaderFixture, ReadColorConfiguration) {

    ConfigurationReader &cr = ConfigurationReaderTest::getReader();
    Color clr = cr.getColor("colorVar", false);
    ASSERT_FLOAT_EQ(clr.x, 255);
    ASSERT_FLOAT_EQ(clr.y, 10);
    ASSERT_FLOAT_EQ(clr.z, 25);
    clr = cr.getColor("colorVar", true);
    ASSERT_FLOAT_EQ(clr.x, 255.f/255);
    ASSERT_FLOAT_EQ(clr.y, 10.f/255);
    ASSERT_FLOAT_EQ(clr.z, 25.f/255);

    EXPECT_THROW({
        try {
            clr = cr.getColor("invalidColorVar1");
        } catch (std::runtime_error& ex) {
            throw;
        }
    }, std::runtime_error) << "Expected a runtime error for improperly formatted color entries";

    EXPECT_THROW({
         try {
             clr = cr.getColor("invalidColorVar2");
         } catch (std::runtime_error& ex) {
             throw;
         }
     }, std::runtime_error) << "Expected a runtime error for improperly formatted color entries";
}

TEST_F(ConfigurationReaderFixture, InvalidConfigFile) {

}

TEST_F(ConfigurationReaderFixture, InvalidConfigEntry) {
    ConfigurationReader& cr = ConfigurationReaderTest::getReader();
    EXPECT_THROW({
        try {
            auto booleanValue = cr.getBoolean("boolVarThatDoesntExist");
        } catch (std::runtime_error& ex) {
            throw;
        }
    }, std::runtime_error) << "Expected an exception for invalid keys!" << endl;
}

TEST_F(ConfigurationReaderFixture, Comments) {
    ConfigurationReader& cr = ConfigurationReaderTest::getReader();
    ASSERT_EQ(ConfigurationReaderTest::getNumberOfKeysStartingWith('#', cr), 0) << "Comments not ignored";

}