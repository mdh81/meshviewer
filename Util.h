#ifndef UTIL_H
#define UTIL_H
#include <cmath>
#include <iostream>

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
        
        template<typename T>
        static void printMemory(const T* pData, const size_t numBytes, std::ostream& os) {
            os << "Printing " << numBytes << " bytes of memory " << (size_t) pData << std::endl; 
            size_t numEntries = numBytes / sizeof(T);
            for (size_t i = 0; i < numEntries; ++i, ++pData) {
                os << (size_t) pData << ": " << *pData << std::endl;
            }
        } 

};

}


#endif
