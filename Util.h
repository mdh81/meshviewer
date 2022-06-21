#ifndef UTIL_H
#define UTIL_H
#include "glm/glm.hpp"

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

        static common::Bounds transformBounds(const common::Bounds& bounds, glm::mat4& transformMatrix) {
            glm::vec4 min(bounds.xmin, bounds.ymin, bounds.zmin, 1.f);
            glm::vec4 max(bounds.xmax, bounds.ymax, bounds.zmax, 1.f);
            glm::vec4 newMin = transformMatrix * min;
            glm::vec4 newMax = transformMatrix * max;
            return common::Bounds{newMin.x, newMin.y, newMin.z, newMax.x, newMax.y, newMax.z};
        }

};

}


#endif
