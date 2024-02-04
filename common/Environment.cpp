#include "Environment.h"
#include <stdexcept>
#include <cstring>

extern char **environ;

namespace mv::common {
    void Environment::loadEnvironment() {
#ifdef _WIN32
        throw std::runtime_error("Environment::loadEnvironment() needs to be implemented for Windows");
#else
        for (char **env = environ; *env != nullptr; ++env) {
            if (!strncmp(*env, "mv_", 3)) {
                std::string envVariable = *env + 3;
                auto splitPos = envVariable.find('=');
                if (splitPos == envVariable.npos) {
                    throw std::runtime_error("Expected environment variable definitions to use form 'variable=value'"
                                             + envVariable + " does not adhere to this convention");
                }
                environmentVariables.insert(envVariable.substr(0, splitPos));
            }
        }
#endif
    }
}
