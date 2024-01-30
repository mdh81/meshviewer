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
       // Inherit constructors
       using std::shared_ptr<T>::shared_ptr;

        template<typename... ArgumentTypes>
        SharedPointer(ArgumentTypes&&... arguments) : std::shared_ptr<T>(std::forward<ArgumentTypes>(arguments)...) {}

       // Conversion constructor to build a shared pointer of base type from a raw pointer to a derived instance
       template<typename DerivedType>
       SharedPointer(DerivedType* derivedPtr) : std::shared_ptr<T>(static_cast<T*>(derivedPtr)) {}

       // Conversion constructor to build a shared pointer from a weak pointer of the same type
       SharedPointer(std::weak_ptr<T> const& weakPtr) : std::shared_ptr<T>{weakPtr.lock()} {
       }

       // Down cast to a weak pointer of derived type
       template<typename Derived>
       std::weak_ptr<Derived> asWeakPointer() const {
           static_assert(std::is_base_of_v<T, Derived>, "Can only downcast to inherited types");
           auto derivedPointer = std::dynamic_pointer_cast<Derived>(*this);
           return std::weak_ptr<Derived>(derivedPointer);
       }

    };

    // A unique pointer wrapper
    template<typename T>
    struct UniquePointer : SmartPointer, std::unique_ptr<T> {
        // Inherit constructors
        using std::unique_ptr<T>::unique_ptr;

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