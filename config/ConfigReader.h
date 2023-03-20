#ifndef MESH_VIEWER_CONFIG_READER_H
#define MESH_VIEWER_CONFIG_READER_H

#include <iostream>
#include <string>
#include <unordered_map>

namespace mv { namespace config {

class ConfigReader {
    public:
        static ConfigReader& getInstance() {
            static ConfigReader cfgReader("config/defaults.cfg");
            return cfgReader;
        }

    private:
       explicit ConfigReader(const std::string& configFileName);

    public:
       ~ConfigReader() = default;
       std::string getValue(const std::string& name);
       bool getBoolean(std::string const& name);

    private:
       ConfigReader(const ConfigReader&) = delete;
       ConfigReader(ConfigReader&&) = delete;
       ConfigReader& operator=(const ConfigReader&) = delete;
       ConfigReader& operator=(ConfigReader&&) = delete;

    private:
       std::unordered_map<std::string, std::string> m_data;
       std::string m_fileName;
       friend class ConfigReaderTest;
};

} }


#endif
