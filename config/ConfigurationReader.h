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
       std::string getValue(std::string const& name) const;

       template<typename T>
       [[nodiscard]]
       T getValueAs(std::string const& name) const {
           if (std::is_floating_point_v<T>) {
               return static_cast<T>(std::strtof(getValue(name).c_str(), nullptr));
           } else if (std::is_integral_v<T> && std::is_unsigned_v<T>) {
               return static_cast<T>(std::strtoll(getValue(name).c_str(), nullptr, 10));
           } else if (std::is_integral_v<T>) {
               return static_cast<T>(std::strtoull(getValue(name).c_str(), nullptr, 10));
           } else {
               throw std::runtime_error(std::string{"getValueAs is not supported for "} + typeid(T).name());
           }
       }
       [[nodiscard]]
       bool getBoolean(std::string const& name) const;
       [[nodiscard]]
       common::RGBColor getColor(std::string const& name, bool normalize=true) const;
       [[nodiscard]]
       common::Vector3D getVector(std::string const& name) const;
       void append(std::string const& name, std::string const& value) {
           data.emplace(name, value);
       }

    public:
       ConfigurationReader(ConfigurationReader const&) = delete;
       ConfigurationReader(ConfigurationReader&&) = delete;
       ConfigurationReader& operator=(const ConfigurationReader&) = delete;
       ConfigurationReader& operator=(ConfigurationReader&&) = delete;

    private:
       std::unordered_map<std::string, std::string> data;
       std::string fileName;
       friend class ConfigurationReaderTest;
};

}