#include "ConfigurationReader.h"
#include <exception>
#include <fstream>
#include <sstream>
using namespace std;
using namespace mv::common;

namespace mv { namespace config {

ConfigurationReader::ConfigurationReader(const std::string& fileName) : m_fileName(fileName) {
    
    ifstream ifs(m_fileName);
    if (!ifs)
        throw runtime_error("Unable to open config file " + fileName); 
   
    // TODO: Add handling for entries with whitespace 
    while (ifs) {
        string config; 
        ifs >> config;
        if (config.empty() || config.at(0) == '#') continue;
        auto tokEnd = config.find('=');
        if (tokEnd == string::npos) 
            throw runtime_error("Invalid config entry: " + config); 
        m_data.emplace(config.substr(0, tokEnd), config.substr(tokEnd+1));
    }
    ifs.close();
}

std::string ConfigurationReader::getValue(const std::string& name) {
    auto itr = m_data.find(name);
    if (itr != m_data.end()) {
        return itr->second;
    } else {
        throw runtime_error("No configuration found for " + name); 
    }
}

bool ConfigurationReader::getBoolean(std::string const& name) {
    std::string val = getValue(name);
    if (val == "true" || val == "True")
        return true;
    else if (val == "false" || val == "False")
        return false;
    else
        throw runtime_error(name + " is not a boolean property");
}

Color ConfigurationReader::getColor(const std::string &name, const bool normalize) {
    std::string val = getValue(name);
    Color color;
    istringstream ss(val);
    string colorComponent;
    auto formattedCorrectly = true;
    auto* colorData = reinterpret_cast<float*>(&color);
    for (common::byte i = 0; i < 3 && formattedCorrectly; ++i) {
        if ((formattedCorrectly = (getline(ss, colorComponent, ',').operator bool() && !colorComponent.empty()))) {
            colorData[i] = stof(colorComponent);
            colorData[i] /= normalize ? 255 : 1;
        }
    }
    if (!formattedCorrectly) {
        throw std::runtime_error(name + "is not property formatted. Use format \"<r,g,b>\"");
    }
    return color;
}


} }
