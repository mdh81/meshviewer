#ifndef TYPES_H
#define TYPES_H
#include <vector>
#include <limits>
#include <type_traits>
#include <iostream>
#include <cmath>
#include <memory>
#include <optional>
#include "GL/GLEW.h"

// Definitions of types that are common among various pieces of the meshviewer application
namespace mv::common {

struct Point3D {
    float x;
    float y;
    float z; 
};
// TODO: replace by mathlib's Vector
using Vector = Point3D;
using Color = Point3D;

inline std::ostream& operator<<(std::ostream& os, const Point3D& v) {
    os << "[" << v.x << "," << v.y << "," << v.z << "]"; 
    return os;
}

template<typename T>
inline std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec) {
    for (auto& item : vec)
        os << item << ' ';
    return os;
}

// A 2D array whose data can be shared among instances of this type. The data
// is stored as a 1D array to facilitate easy transfer to OpenGL APIs
template<typename T, unsigned tupleSize=1>
class Array {
    private:
        size_t m_size;
        size_t m_offset;
        std::shared_ptr<T[]> m_data;
    public:
    [[maybe_unused]] explicit Array(size_t numElements)
            : m_size(numElements)
            , m_offset(0)
            , m_data(new T[m_size * tupleSize], std::default_delete<T[]>()) {
        }
        
        size_t getSize() const { return m_size; }
        
        size_t getDataSize() const { return m_size * tupleSize * sizeof(T); }
        
        T const* getData() const { return m_data.get(); }
        
        void append(const std::initializer_list<T>& d) {
            if (d.size() != tupleSize) {
                throw std::invalid_argument("Tuple size is " + 
                                            std::to_string(tupleSize) +
                                            ". Input size is " + 
                                            std::to_string(d.size()));
            }
            memcpy(m_data.get() + m_offset, data(d), sizeof(T) * tupleSize);
            m_offset += tupleSize;
        }
};

struct Bounds {
    float xmin, xmax;
    float ymin, ymax;
    float zmin, zmax;
    
    Bounds() { 
        xmin = std::numeric_limits<float>::max();
        ymin = std::numeric_limits<float>::max();
        zmin = std::numeric_limits<float>::max();
        xmax = -xmin;
        ymax = -ymin;
        zmax = -zmin;
    }

    // Builds a symmetric bounding box where each side is
    // of the specified length 
    explicit Bounds(float sideLength) {
        xmin = -0.5f * sideLength;
        xmax = +0.5f * sideLength;
        ymin = -0.5f * sideLength;
        ymax = +0.5f * sideLength;
        zmin = -0.5f * sideLength;
        zmax = +0.5f * sideLength;
    }

    Bounds(float xmi, float ymi, float zmi, float xmx, float ymx, float zmx) :
        xmin(xmi), ymin(ymi), zmin(zmi), xmax(xmx), ymax(ymx), zmax(zmx) { }
    float xlen() const { return xmax - xmin; }
    float ylen() const { return ymax - ymin; }
    float zlen() const { return zmax - zmin; }
    float xmid() const { return (xmin + xmax)*0.5; } 
    float ymid() const { return (ymin + ymax)*0.5; } 
    float zmid() const { return (zmin + zmax)*0.5; } 
    float len() const  { return sqrt( ((xmax - xmin) * (xmax - xmin)) + 
                                      ((ymax - ymin) * (ymax - ymin)) + 
                                      ((zmax - zmin) * (zmax - zmin)) ); }
};

inline std::ostream& operator<<(std::ostream& os, const Bounds& b) {
    os << "Min:[" << b.xmin << "," << b.ymin << "," << b.zmin << "] "; 
    os << "Max:[" << b.xmax << "," << b.ymax << "," << b.zmax << "]";
    return os;
}

using VertexIndices = std::vector<unsigned>;
using VertexIndex = unsigned;

template <typename T, T beginVal, T endVal>
class EnumIterator {
    // Underlying type's alias
    using ULType = typename std::underlying_type<T>::type;
    // An integer to maintain the iterator's position
    ULType m_val;
    public:
        // Conversion constructor to convert enumerator to iterator type
        explicit EnumIterator(const T& f) : m_val(static_cast<ULType>(f)) {}
        // Default constructor that initilizes the iterator to point to the first enumerator
        EnumIterator() : m_val(static_cast<ULType>(beginVal)) {}
        // Postfix increment operator
        EnumIterator operator++() {
            ++m_val;
            return *this;
        }
        // Dereference operator 
        T operator*() { return static_cast<T>(m_val); }
        // Begin and end methods
        EnumIterator begin() { return *this; }
        EnumIterator end() { return ++EnumIterator(endVal); }
        bool operator!=(const EnumIterator& other) { return m_val != other.m_val; }
};

using byte = char;

enum class Axis {
    X,
    Y,
    Z,
    Arbitrary
};

struct WindowDimensions {
    unsigned width;
    unsigned height;
};

enum class NormalLocation {
    Face,
    Vertex
};

struct GlyphDecorator {
    enum class Style {
        Line,
        Arrow
    };
    Style style;
    Color color;
    float length;
    std::optional<float> arrowRadius;
    std::optional<float> arrowLength;
};

enum class GlyphAssociation {
    Undefined,
    FaceNormal,
    VertexNormal
};

static std::string getGLErrorString(GLint glError) {
    std::string errStr;
    switch (glError) {
        case GL_INVALID_ENUM:
            errStr = "Invalid enum";
            break;
        case GL_INVALID_VALUE:
            errStr = "Invalid value";
            break;
        case GL_INVALID_OPERATION:
            errStr = "Invalid operation";
            break;
        default:
            errStr = std::to_string(glError);
    }
    return errStr;
}

static GLuint glError = 0;
#define checkGLError(glFunc)                                         \
    glError = glGetError();                                          \
    if (glError) {                                                   \
        cout << #glFunc << " returned " << getGLErrorString(glError) \
             << " at line " << __LINE__ << " of "                    \
             << __FILE__ << endl;                                    \
        std::terminate();                                            \
    }

#define glCallWithErrorCheck(glFunc, glArgs...) \
    glFunc(glArgs);                             \
    checkGLError(glFunc)
}
#endif
