#include "ConfigurationReader.h"
#include <fstream>
#include <sstream>
using namespace std;
using namespace mv::common;

namespace mv::config {

        ConfigurationReader::ConfigurationReader(const std::string &fileName) : fileName(fileName) {

            ifstream ifs(fileName);
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
                data.emplace(config.substr(0, tokEnd), config.substr(tokEnd + 1));
            }
            ifs.close();
        }

        std::string ConfigurationReader::getValue(const std::string &name) const {
            auto itr = data.find(name);
            if (itr != data.end()) {
                return itr->second;
            } else {
                throw runtime_error("No configuration found for " + name);
            }
        }

        bool ConfigurationReader::getBoolean(std::string const &name) const {
            std::string val = getValue(name);
            if (val == "true" || val == "True")
                return true;
            else if (val == "false" || val == "False")
                return false;
            else
                throw runtime_error(name + " is not a boolean property");
        }

        RGBColor ConfigurationReader::getColor(const std::string &name, const bool normalize) const {
            std::string val = getValue(name);
            RGBColor color;
            istringstream ss(val);
            string colorComponent;
            auto formattedCorrectly = true;
            for (common::byte i = 0; i < 3 && formattedCorrectly; ++i) {
                if ((formattedCorrectly = (getline(ss, colorComponent, ',').operator bool() &&
                                           !colorComponent.empty()))) {
                    color[i] = stof(colorComponent);
                    color[i] /= normalize ? 255 : 1;
                }
            }
            if (!formattedCorrectly) {
                throw std::runtime_error(name + "is not property formatted. Use format \"<r,g,b>\"");
            }
            return color;
        }

        Vector3D ConfigurationReader::getVector(const std::string &name) const {
            std::string val = getValue(name);
            Vector3D vector;
            istringstream ss(val);
            string positionStr;
            auto formattedCorrectly = true;
            for (common::byte i = 0; i < 3 && formattedCorrectly; ++i) {
                if ((formattedCorrectly = (getline(ss, positionStr, ',').operator bool() &&
                                           !positionStr.empty()))) {
                    vector[i] = stof(positionStr);
                }
            }
            if (!formattedCorrectly) {
                throw std::runtime_error(name + "is not property formatted. Use format \"<x,y,z>\"");
            }
            return vector;
        }
}
