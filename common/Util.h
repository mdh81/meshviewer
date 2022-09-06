#ifndef UTIL_H
#define UTIL_H
#include "glm/glm.hpp"

#include <cmath>
#include <iostream>
#include <fstream>
#include <iomanip>

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

        static common::Bounds transformBounds(const common::Bounds& bounds, const glm::mat4& transformMatrix) {
            glm::vec4 min(bounds.xmin, bounds.ymin, bounds.zmin, 1.f);
            glm::vec4 max(bounds.xmax, bounds.ymax, bounds.zmax, 1.f);
            glm::vec4 newMin = transformMatrix * min;
            glm::vec4 newMax = transformMatrix * max;
            return common::Bounds{newMin.x, newMin.y, newMin.z, newMax.x, newMax.y, newMax.z};
        }

        static void printMatrix(glm::mat4& matrix) {
            using namespace std;
            cout << setw(10) << matrix[0][0] << setw(10) << matrix[1][0] << setw(10) << matrix[2][0] << setw(10) << matrix[3][0] 
                 << setw(10) << endl;
            cout << setw(10) << matrix[0][1] << setw(10) << matrix[1][1] << setw(10) << matrix[2][1] << setw(10) << matrix[3][1]
                 << setw(10) << endl;
            cout << setw(10) << matrix[0][2] << setw(10) << matrix[1][2] << setw(10) << matrix[2][2] << setw(10) << matrix[3][2]
                 << setw(10) << endl;
            cout << setw(10) << matrix[0][3] << setw(10) << matrix[1][3] << setw(10) << matrix[2][3] << setw(10) << matrix[3][3]
                 << endl;
        }

        static void writeBounds(const std::string& fileName, const common::Bounds& bounds) {
            std::string header("STL Generated by Mesh Viewer");
            std::ofstream ofs(fileName, std::ios::binary);
            ofs.write(header.c_str(), 80);
            unsigned numTris = 12;
            ofs.write(reinterpret_cast<char*>(&numTris), 4);
            float normal[3], v1[3], v2[3], v3[3];
            auto writeTri = [&]() {
                ofs.write(reinterpret_cast<char*>(normal), 12);
                ofs.write(reinterpret_cast<char*>(v1), 12);
                ofs.write(reinterpret_cast<char*>(v2), 12);
                ofs.write(reinterpret_cast<char*>(v3), 12);
                unsigned short dummy = 0;
                ofs.write(reinterpret_cast<char*>(&dummy), 2);
            };
            
            // Back face
            normal[0] = 0.f; normal[1] = 0.f; normal[2] = -1.f;
            v1[0] = bounds.xmin; v1[1] = bounds.ymax; v1[2] = bounds.zmin;
            v2[0] = bounds.xmax; v2[1] = bounds.ymin; v2[2] = bounds.zmin;
            v3[0] = bounds.xmin; v3[1] = bounds.ymin; v3[2] = bounds.zmin;
            writeTri();
            v1[0] = bounds.xmin; v1[1] = bounds.ymax; v1[2] = bounds.zmin;
            v2[0] = bounds.xmax; v2[1] = bounds.ymax; v2[2] = bounds.zmin;
            v3[0] = bounds.xmax; v3[1] = bounds.ymin; v3[2] = bounds.zmin;
            writeTri();

            // Front face
            normal[0] = 0.f; normal[1] = 0.f; normal[2] = 1.f;
            v1[0] = bounds.xmin; v1[1] = bounds.ymin; v1[2] = bounds.zmax;
            v2[0] = bounds.xmax; v2[1] = bounds.ymin; v2[2] = bounds.zmax;
            v3[0] = bounds.xmin; v3[1] = bounds.ymax; v3[2] = bounds.zmax;
            writeTri();
            v1[0] = bounds.xmax; v1[1] = bounds.ymin; v1[2] = bounds.zmax;
            v2[0] = bounds.xmax; v2[1] = bounds.ymax; v2[2] = bounds.zmax;
            v3[0] = bounds.xmin; v3[1] = bounds.ymax; v3[2] = bounds.zmax;
            writeTri();

            // Right Face
            normal[0] = 1.f; normal[1] = 0.f; normal[2] = 0.f;
            v1[0] = bounds.xmax; v1[1] = bounds.ymin; v1[2] = bounds.zmax;
            v2[0] = bounds.xmax; v2[1] = bounds.ymin; v2[2] = bounds.zmin;
            v3[0] = bounds.xmax; v3[1] = bounds.ymax; v3[2] = bounds.zmin;
            writeTri();
            v1[0] = bounds.xmax; v1[1] = bounds.ymin; v1[2] = bounds.zmax;
            v2[0] = bounds.xmax; v2[1] = bounds.ymax; v2[2] = bounds.zmin;
            v3[0] = bounds.xmax; v3[1] = bounds.ymax; v3[2] = bounds.zmax;
            writeTri();

            // Left Face
            normal[0] = -1.f; normal[1] = 0.f; normal[2] = 0.f;
            v1[0] = bounds.xmin; v1[1] = bounds.ymin; v1[2] = bounds.zmax;
            v2[0] = bounds.xmin; v2[1] = bounds.ymax; v2[2] = bounds.zmin;
            v3[0] = bounds.xmin; v3[1] = bounds.ymin; v3[2] = bounds.zmin;
            writeTri();
            v1[0] = bounds.xmin; v1[1] = bounds.ymin; v1[2] = bounds.zmax;
            v2[0] = bounds.xmin; v2[1] = bounds.ymax; v2[2] = bounds.zmax;
            v3[0] = bounds.xmin; v3[1] = bounds.ymax; v3[2] = bounds.zmin;
            writeTri();

            // Top Face
            normal[0] = 0.f; normal[1] = 1.f; normal[2] = 0.f;
            v1[0] = bounds.xmin; v1[1] = bounds.ymax; v1[2] = bounds.zmax;
            v2[0] = bounds.xmax; v2[1] = bounds.ymax; v2[2] = bounds.zmax;
            v3[0] = bounds.xmax; v3[1] = bounds.ymax; v3[2] = bounds.zmin;
            writeTri();
            v1[0] = bounds.xmin; v1[1] = bounds.ymax; v1[2] = bounds.zmax;
            v2[0] = bounds.xmax; v2[1] = bounds.ymax; v2[2] = bounds.zmin;
            v3[0] = bounds.xmin; v3[1] = bounds.ymax; v3[2] = bounds.zmin;
            writeTri();

            // Bottom Face
            normal[0] = 0.f; normal[1] = -1.f; normal[2] = 0.f;
            v1[0] = bounds.xmin; v1[1] = bounds.ymin; v1[2] = bounds.zmax;
            v2[0] = bounds.xmax; v2[1] = bounds.ymin; v2[2] = bounds.zmax;
            v3[0] = bounds.xmax; v3[1] = bounds.ymin; v3[2] = bounds.zmin;
            writeTri();
            v1[0] = bounds.xmin; v1[1] = bounds.ymin; v1[2] = bounds.zmax;
            v2[0] = bounds.xmax; v2[1] = bounds.ymin; v2[2] = bounds.zmin;
            v3[0] = bounds.xmin; v3[1] = bounds.ymin; v3[2] = bounds.zmin;
            writeTri();

            ofs.close();
        }
};

}


#endif