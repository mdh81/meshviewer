#pragma once

#include <memory>

namespace mv::common {

    // Dummy base class to enforce smart pointer only constraint in the hasher and comparator below without relying on
    // the hideous syntax of "template template" parameter. Construction semantics is protected logic to prevent the app
    // from creating instances of this dummy class
    struct SmartPointer {
    protected:
        SmartPointer() = default;
        SmartPointer(SmartPointer const&) = default;
        SmartPointer(SmartPointer&&) = default;
        SmartPointer& operator=(SmartPointer const&) = default;
        SmartPointer& operator=(SmartPointer&&) = default;
    };

    // A shared pointer wrapper
    template<typename T>
    struct SharedPointer : SmartPointer, std::shared_ptr<T> {
        template<typename... ArgumentTypes>
        SharedPointer(ArgumentTypes&&... arguments) : std::shared_ptr<T>(std::forward<ArgumentTypes>(arguments)...) {}
    };

    // A unique pointer wrapper
    template<typename T>
    struct UniquePointer : SmartPointer, std::unique_ptr<T> {
        template<typename... ArgumentTypes>
        UniquePointer(ArgumentTypes&&... arguments) : std::unique_ptr<T>(std::forward<ArgumentTypes>(arguments)...) {}
    };

    // Hash value for a smart pointer is the address of the pointee
    struct SmartPointerHasher {
        template<typename T>
        size_t operator()(T const& smartPtr) const {
            static_assert(std::is_base_of_v<SmartPointer, T>, "Only smart pointers can be hashed with "
                                                              "SmartPointerLookupHelper::Hasher");
            return reinterpret_cast<size_t>(smartPtr.get());
        }
    };

    // Two smart pointers are equal if they point to the same object
    struct SmartPointerComparator {
        template<typename T>
        bool operator()(T const& smartPtrA, T const& smartPtrB) const {
            static_assert(std::is_base_of_v<SmartPointer, T>, "Only smart pointers can be compared with "
                                                              "SmartPointerLookupHelper::Comparator");
            return smartPtrA.get() == smartPtrB.get();
        }
    };
}