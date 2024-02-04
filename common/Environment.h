#pragma once
#include <string>
#include <unordered_set>
namespace mv::common {
    class Environment {
    public:
        bool isVariableDefined(std::string const &variableName) {
            if (environmentVariables.empty()) {
                loadEnvironment();
            }
            return environmentVariables.find(variableName) != environmentVariables.end();
        }

        // Cache the variable value in a static variable to speed up this query that
        // is made for each GL call
        bool isUnitTestingInProgress() {
            static bool isUnitTestingInProgress = isVariableDefined("UNIT_TESTING_IN_PROGRESS");
            return isUnitTestingInProgress;
        }

    private:
        static inline std::unordered_set<std::string> environmentVariables{};
        void loadEnvironment();
    };
}