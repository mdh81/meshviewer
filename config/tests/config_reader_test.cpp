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
        ofs.close();
    }
};


TEST_F(ConfigReaderFixture, ReadConfig) {
    ConfigReader cr = ConfigReader("./test.cfg"); 
    ASSERT_EQ(cr.getValue("varA"), "valA") << "Config map has unexpected state" << endl;

}

TEST_F(ConfigReaderFixture, InvalidConfigFile) {

}

TEST_F(ConfigReaderFixture, InvalidConfigEntry) {

}
