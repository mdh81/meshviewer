#include <array>
#include <iostream>

template<typename T, unsigned size>
class Vector {
    static_assert(size > 1, "Size of a vector should be greater than 1");

// Proxy to an element in the vector
    struct Proxy;
public:
    // Vector data accessors that allow 1,2, 3 and 4D vectors to be used in semantically meaningful fashion.
    // For example:
    //      using Point2D = Vector<float, 2>
    //      Point2D origin{10, 10};
    //      origin.x += 10
    //      origin.y += 10;
    // or
    //      using RGBColor = Vector<float,3>
    //      RGBColor color{RED, GREEN, BLUE}
    //      glClearColor(color.r, color.g, color.b);
    union {Proxy x; Proxy r;};
    union {Proxy y; Proxy g;};
    union {Proxy z; Proxy b;};
    union {Proxy w; Proxy a;};

private:
// Definition of element proxy. Proxy is defined private so clients of vector cannot create proxies external to a vector
    struct Proxy {
        // Update element pointed to by the proxy to T. value is const T& to accept both l and rvalues
        // e.g.
        // using Point3D = Vector<float,3>;
        // Point3D p1, p2;
        // p1.x = 100.f
        // or
        // p2.x = p1.x;
        void operator=(T const& value) {
            if (!data) {
                throw std::runtime_error("Unbound proxy!");
            }
            *data = value;
        }
        // Convert to T so a proxy can be used in expressions that involve T
        // e.g.
        // void someFunction(float val);
        // using Point3D = Vector<float,3>;
        // Point3D p1;
        // someFunction(p1.x);
        T operator()() {
            return *data;
        }
        // Create an empty proxy
        Proxy() : data(nullptr) {
        }
        // Create a proxy the given element
        Proxy(T& element) : data(&element) {
        }

    private:
        // Clear the proxy. Permits a Vector to be moved meaningfully by allowing the r-value that's moved to have its proxies
        // null-ed out. Defined private to prevent clients of vector from clobbering the internal state of a vector
        void reset() {
            data = nullptr;
        }

        T* data;

// Allow the outer class Vector to call reset() to facilitate move operations
        friend Vector<T, size>;
    };

// These macros prevent code duplication in vector's constructors and operators
#define INIT_CONVENIENCE_MEMBERS          \
    switch (size) {                       \
    case 1:                               \
        x = Proxy(data[0]);               \
    case 2:                               \
        y = Proxy(data[1]);               \
    case 3:                               \
        z = Proxy(data[2]);               \
    case 4:                               \
        w = Proxy(data[3]);               \
    default:                              \
        break;                            \
    }

#define RESET_CONVENIENCE_MEMBERS(Vector) \
    Vector.x.reset();                     \
    Vector.y.reset();                     \
    Vector.z.reset();                     \
    Vector.w.reset();

public:
    Vector() {
        INIT_CONVENIENCE_MEMBERS;
    }

    Vector(Vector const& another) {
        operator=(another);
    }

    Vector& operator=(Vector const& another) {
        for(auto i = 0; i < size; ++i) {
            data[i] = another.data[i];
        }
        INIT_CONVENIENCE_MEMBERS;
        return *this;
    }

    Vector(Vector&& another) {
        operator=(another);
    }

    Vector& operator=(Vector&& another) {
        data = std::move(another.data);
        INIT_CONVENIENCE_MEMBERS;
        RESET_CONVENIENCE_MEMBERS(another);
        return *this;
    }

    void print(char const* prefix = nullptr) {
        if (prefix) printf("%s: ", prefix);
        for (size_t i = 0; i < size; ++i)
            printf("%f ", data[i]);
        printf("\n");
    }

private:
    std::array<T, size> data {};

};