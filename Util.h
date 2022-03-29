#ifndef UTIL_H
#define UTIL_H
#include <cmath>

namespace meshviewer {

// TODO: Move to MathLib
class Util {
    public:
        static bool areFloatsEqual(float a, float b) {
            return fabs(a-b) < sm_tolerance; 
        }
        static bool isLessOrEqual(float a, float b) {
            return (a < b || fabs(a-b) < sm_tolerance);
        }
        static bool isGreaterOrEqual(float a, float b) {
            return (a > b || fabs(a-b) < sm_tolerance);
        }
        static constexpr float sm_tolerance = 1e-6;

        

};

}


#endif
