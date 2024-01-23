#pragma once

#include "Callbacks.h"
#include "MeshViewerObject.h"
#include "PointerTypes.h"
#include <unordered_set>

namespace mv::events {

class CallbackFactory {

public:
    template<typename InstanceType, typename FunctionPointerType, typename... ArgumentTypes>
    BasicEventCallback::Pointer const& createCallback(InstanceType& instance, FunctionPointerType functionPointer, ArgumentTypes&&... arguments) {
        if (sizeof...(ArgumentTypes)) {
            return createBasicEventCallback(instance, functionPointer, std::forward<ArgumentTypes>(arguments)...);
        }
    }

    void removeCallback(BasicEventCallback::Pointer const& basicEventCallback) {
        basicEventCallbacks.erase(basicEventCallbacks.find(basicEventCallback));
    }

private:
    template<typename InstanceType, typename FunctionPointerType, typename... ArgumentTypes>
    BasicEventCallback::Pointer const& createBasicEventCallback(InstanceType& instance, FunctionPointerType functionPointer, ArgumentTypes&&... arguments) {
        auto [itr, status] = basicEventCallbacks.emplace(new BasicEventCallbackImpl(instance, functionPointer, std::forward<ArgumentTypes>(arguments)...));
        return *itr;
    }

private:
    static inline
    std::unordered_set<
            BasicEventCallback::Pointer,
            common::SmartPointerHasher,
            common::SmartPointerComparator> basicEventCallbacks{};

    friend class CallbackFactoryTester;
};

}
