#include "ConfigReader.h"
#include <exception>
#include <fstream>
using namespace std;

namespace meshviewer { namespace config {

ConfigReader::ConfigReader(const std::string& fileName) : m_fileName(fileName) {
    
    ifstream ifs(m_fileName);
    if (!ifs)
        throw runtime_error("Unable to open config file " + fileName); 
   
    // TODO: Add handling for entries with whitespace 
    while (ifs) {
        string config; 
        ifs >> config;
        if (config.empty()) continue;
        auto tokEnd = config.find('=');
        if (tokEnd == string::npos) 
            throw runtime_error("Invalid config entry: " + config); 
        m_data.emplace(config.substr(0, tokEnd), config.substr(tokEnd+1));
    }
    ifs.close();
}

std::string ConfigReader::getValue(const std::string& name) {
    for (auto& entry : m_data) {
        cerr << entry.first << "==>" << entry.second << endl; 
    }
    auto itr = m_data.find(name);
    if (itr != m_data.end()) {
        return itr->second;
    } else {
        throw runtime_error("No configuration found for " + name); 
    }
}


} }
