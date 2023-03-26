#ifndef MESH_VIEWER_CONFIG_READER_H
#define MESH_VIEWER_CONFIG_READER_H

#include <iostream>
#include <string>
#include <unordered_map>
#include "Types.h"

namespace mv { namespace config {

class ConfigurationReader {
    public:
        static ConfigurationReader& getInstance() {
            static ConfigurationReader cfgReader("config/defaults.cfg");
            return cfgReader;
        }

    private:
       explicit ConfigurationReader(const std::string& configFileName);

    public:
       ~ConfigurationReader() = default;
       std::string getValue(const std::string& name);
       bool getBoolean(std::string const& name);
       common::Color getColor(std::string const& name, bool const normalize=true);

    private:
       ConfigurationReader(const ConfigurationReader&) = delete;
       ConfigurationReader(ConfigurationReader&&) = delete;
       ConfigurationReader& operator=(const ConfigurationReader&) = delete;
       ConfigurationReader& operator=(ConfigurationReader&&) = delete;

    private:
       std::unordered_map<std::string, std::string> m_data;
       std::string m_fileName;
       friend class ConfigurationReaderTest;
};

} }


#endif
