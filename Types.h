#ifndef TYPES_H
#define TYPES_H
#include <vector>
#include <limits>
#include <type_traits>
#include <iostream>

// Definitions of types that are common among various pieces of the meshviewer application
namespace meshviewer { namespace common {

struct Vertex {
    float x;
    float y;
    float z;
};

inline std::ostream& operator<<(std::ostream& os, const Vertex& v) {
    os << "[" << v.x << "," << v.y << "," << v.z << "]"; 
    return os;
}

using Vertices = std::vector<Vertex>; 
using Face = std::vector<unsigned>;
using Faces = std::vector<Face>;

inline std::ostream& operator<<(std::ostream& os, const Face& f) {
    os << '[';
    for (size_t i = 0; i < f.size(); ++i) { 
        os << f.at(i) << (i == f.size() - 1 ? ']' : ','); 
    }
    return os;
}

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
    Bounds(float xmi, float ymi, float zmi, float xmx, float ymx, float zmx) :
        xmin(xmi), ymin(ymi), zmin(zmi), xmax(xmx), ymax(ymx), zmax(zmx) { }
    float xlen() const { return xmax - xmin; }
    float ylen() const { return ymax - ymin; }
    float zlen() const { return zmax - zmin; }
    float xmid() const { return (xmin + xmax)*0.5; } 
    float ymid() const { return (ymin + ymax)*0.5; } 
    float zmid() const { return (zmin + zmax)*0.5; } 
};

inline std::ostream& operator<<(std::ostream& os, const Bounds& b) {
    os << "Min:[" << b.xmin << "," << b.ymin << "," << b.zmin << "] "; 
    os << "Max:[" << b.xmax << "," << b.ymax << "," << b.zmax << "]" << std::endl;
    return os;
}

using VertexIndices = std::vector<unsigned>;

template <typename T, T beginVal, T endVal>
class EnumIterator {
    // Underlying type's alias
    using ULType = typename std::underlying_type<T>::type;
    // An integer to maintain the iterator's position
    ULType m_val;
    public:
        // Conversion constructor to convert enumerator to iterator type
        EnumIterator(const T& f) : m_val(static_cast<ULType>(f)) {}
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

enum class Axis
{
    X,
    Y,
    Z,
    Arbitrary
};

} }
#endif
