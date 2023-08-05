#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include "Types.h"

namespace mv::config {

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
       [[nodiscard]]
       std::string getValue(const std::string& name) const;
       [[nodiscard]]
       bool getBoolean(std::string const& name) const;
       [[nodiscard]]
       common::Color getColor(std::string const& name, bool normalize=true) const;
       [[nodiscard]]
       common::Vector3D getVector(std::string const& name) const;

    public:
       ConfigurationReader(ConfigurationReader const&) = delete;
       ConfigurationReader(ConfigurationReader&&) = delete;
       ConfigurationReader& operator=(const ConfigurationReader&) = delete;
       ConfigurationReader& operator=(ConfigurationReader&&) = delete;

    private:
       std::unordered_map<std::string, std::string> m_data;
       std::string m_fileName;
       friend class ConfigurationReaderTest;
};

}