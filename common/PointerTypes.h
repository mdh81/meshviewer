#pragma once

#include <memory>

namespace mv::common {

    struct SmartPointer {};

    template<typename T>
    struct SharedPointer : SmartPointer, std::shared_ptr<T> {
        // Emplace support constructor
        template<typename... ArgumentTypes>
        SharedPointer(ArgumentTypes&&... arguments) : std::unique_ptr<T>(std::forward<ArgumentTypes>(arguments)...) {}
    };

    template<typename T>
    struct UniquePointer : SmartPointer, std::unique_ptr<T> {
        // Emplace support constructor
        template<typename... ArgumentTypes>
        UniquePointer(ArgumentTypes&&... arguments) : std::unique_ptr<T>(std::forward<ArgumentTypes>(arguments)...) {}
    };

    struct SmartPointerLookupHelper {
        struct Hasher {
            template<typename T>
            size_t operator()(T const& smartPtr) const {
                static_assert(std::is_base_of_v<SmartPointer, T>, "Only smart pointers can be hashed with "
                                                                  "SmartPointerLookupHelper::Hasher");
                return reinterpret_cast<size_t>(smartPtr.get());
            }
        };
        struct Comparator {
            template<typename T>
            bool operator()(T const& smartPtrA, T const& smartPtrB) const {
                static_assert(std::is_base_of_v<SmartPointer, T>, "Only smart pointers can be compared with "
                                                                  "SmartPointerLookupHelper::Comparator");
                return smartPtrA.get() == smartPtrB.get();
            }
        };
    };
    using SmartPointerHasher = SmartPointerLookupHelper::Hasher;
    using SmartPointerComparator = SmartPointerLookupHelper::Comparator;

}