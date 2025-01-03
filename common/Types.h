#pragma once
#include <vector>
#include <limits>
#include <type_traits>
#include <iostream>
#include <cmath>
#include <memory>
#include <optional>
#include <cstring>
#include "3dmath/Vector.h"
#include "3dmath/SupportingTypes.h"
#include "3dmath/OrthographicProjectionMatrix.h"
#include "3dmath/ProjectionMatrix.h"
#include "3dmath/RotationMatrix.h"
#include "OpenGLCall.h"
#include "TransformMatrix.h"

// TODO: Time to split this into multiple headers.

// Definitions of types that are common among various pieces of the meshviewer application
namespace mv::common {

    using Point3D = math3d::Vector3<float>;
    using Vector3D = math3d::Vector3<float>;
    using Point2D = math3d::Vector2<float>;
    using Vector2D = math3d::Vector2<float>;
    using Color = Point3D;
    using RGBAColor = math3d::Vector4<float>;
    using RGBColor = math3d::Vector3<float>;
    using Line = std::vector<Point3D>;
    using Lines = std::vector<Line>;
    using Points = std::vector<Point3D>;
    using TwoDPoints = std::vector<Point2D>;
    using Bounds = math3d::Bounds3D<float>;
    using ProjectionMatrix = math3d::ProjectionMatrix<float>;
    using RotationMatrix = math3d::RotationMatrix<float>;
    using Point2DUniquePointer = std::unique_ptr<Point2D>;
    using ProjectionMatrixPointer = std::shared_ptr<ProjectionMatrix>;
    using Point4D = math3d::Vector<float, 4>;
    using OrthographicProjectionMatrix = math3d::OrthographicProjectionMatrix<float, math3d::CoordinateSystemHandedness::LeftHanded>;
    using Vector4D = math3d::Vector4<float>;

    constexpr const char* AppName = "MeshViewer";

    inline std::ostream &operator<<(std::ostream &os, const Point3D &v) {
        os << "[" << v.x << "," << v.y << "," << v.z << "]";
        return os;
    }

    template<typename T>
    inline std::ostream &operator<<(std::ostream &os, const std::vector<T> &vec) {
        for (auto &item: vec)
            os << item << ' ';
        return os;
    }

// A 2D array whose data can be shared among instances of this type. The data
// is stored as a 1D array to facilitate easy transfer to OpenGL APIs.
    template<typename T, unsigned tupleSize = 1>
    class Array {
    private:
        size_t m_size;
        size_t m_offset;
        std::shared_ptr<T[]> m_data;
    public:
        explicit Array(size_t numElements)
                : m_size(numElements), m_offset(0), m_data(new T[m_size * tupleSize], std::default_delete<T[]>()) {
        }

        [[nodiscard]] size_t getSize() const { return m_size; }

        [[nodiscard]] size_t getDataSize() const { return m_size * tupleSize * sizeof(T); }

        T const *getData() const { return m_data.get(); }

        // TODO: Assert that all the parameters are of the same type
        template<typename... Types>
        void append(Types... args) {
            static_assert(sizeof...(args) == tupleSize, "Tuple size and argument size doesn't match");
            append({args...});
        }

        T operator[](unsigned const index) const {
            if (index < m_size * tupleSize) {
                return m_data[index];
            } else {
                throw std::runtime_error(std::to_string(index) + " is out of bounds");
            }
        }

        T &operator[](unsigned const index) {
            if (index < m_size * tupleSize) {
                return m_data[index];
            } else {
                throw std::runtime_error(std::to_string(index) + " is out of bounds");
            }
        }

    private:
        void append(std::initializer_list<T> const &list) {
            if (list.size() != tupleSize) {
                throw std::invalid_argument("Tuple size is " +
                                            std::to_string(tupleSize) +
                                            ". Input size is " +
                                            std::to_string(list.size()));
            }
            memcpy(m_data.get() + m_offset, data(list), sizeof(T) * tupleSize);
            m_offset += tupleSize;
        }

    };

    using Triangles = std::vector<Array<unsigned, 3>>;

    using VertexIndices = std::vector<unsigned>;
    using VertexIndex = unsigned;

    template<typename T, T beginVal, T endVal>
    class EnumIterator {
        // Underlying type's alias
        using ULType = typename std::underlying_type<T>::type;
        // An integer to maintain the iterator's position
        ULType m_val;
    public:
        // Conversion constructor to convert enumerator to iterator type
        explicit EnumIterator(const T &f) : m_val(static_cast<ULType>(f)) {}

        // Default constructor that initializes the iterator to point to the first enumerator
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

        bool operator!=(const EnumIterator &other) { return m_val != other.m_val; }
    };

    using byte = char;
    using pixel = unsigned char;

// TODO: It makes more sense to move these definitions to specific types (e.g. Glyph)
enum class Axis {
    X,
    Y,
    Z,
    Arbitrary
};

struct DisplayDimensions {
    unsigned windowWidth;
    unsigned windowHeight;
    unsigned frameBufferWidth;
    unsigned frameBufferHeight;
    Point2D normalizedViewportSize;
};

inline std::ostream& operator<<(std::ostream& os, DisplayDimensions const displayDimensions) {
    os << "Window Width " << displayDimensions.windowWidth
       << " Height " << displayDimensions.windowHeight << std::endl;
    os << "Framebuffer Width " << displayDimensions.frameBufferWidth
       << " Height " << displayDimensions.frameBufferHeight << std::endl;
    os << "Normalized viewport size: " << displayDimensions.normalizedViewportSize << std::endl;
    return os;
}

enum class NormalLocation {
    Face,
    Vertex
};

#ifdef EMSCRIPTEN
    using CanvasDimensions = DisplayDimensions;
#endif

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

    enum class Direction {
        Left,
        Right,
        Up,
        Down,
        Forward,
        Backward
    };

    inline
    std::ostream &operator<<(std::ostream &os, Direction const direction) {
        switch (direction) {
            case Direction::Left:
                os << "Left";
                break;
            case Direction::Right:
                os << "Right";
                break;
            case Direction::Up:
                os << "Up";
                break;
            case Direction::Down:
                os << "Down";
                break;
            case Direction::Forward:
                os << "Forward";
                break;
            case Direction::Backward:
                os << "Backward";
                break;
            default:
                os << "Unknown";
        }
        return os;
    }
}