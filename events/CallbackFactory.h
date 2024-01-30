#pragma once

#include "Callbacks.h"
#include "MeshViewerObject.h"
#include "ObjectRegistry.h"
#include "PointerTypes.h"
#include <unordered_set>

namespace mv::events {

class CallbackFactory {

public:
    // NOTE: While these two methods appear like good candidates to be consolidated into a single createCallback() method,
    //       it's critical to note that FunctionPointerType is not compatible between BasicEventCallback and DataEventCallback.
    //       Therefore, we cannot instantiate these two class templates in a function template that has function pointer as
    //       a parameter
    template<typename InstanceType, typename FunctionPointerType, typename... ArgumentTypes>
    BasicEventCallback::ObservingPointer createBasicEventCallback(InstanceType& instance, FunctionPointerType functionPointer, ArgumentTypes&&... arguments) {
        static_assert(std::is_base_of_v<MeshViewerObject, InstanceType>,
                "Callbacks can only be created for instances of MeshViewerObject. "
                "This is to ensure automatic cleanup and to eliminate chances of dangling references");
        auto [itr, status] =
                callbacks.emplace(new BasicEventCallbackImpl(instance, functionPointer, std::forward<ArgumentTypes>(arguments)...));
        notifyCreationObservers(instance, *itr);
        return itr->template asWeakPointer<BasicEventCallback>();
    }

    template<typename InstanceType, typename FunctionPointerType>
    DataEventCallback::ObservingPointer createDataEventCallback(InstanceType& instance, FunctionPointerType functionPointer) {
        static_assert(std::is_base_of_v<MeshViewerObject, InstanceType>,
                      "Callbacks can only be created for instances of MeshViewerObject. "
                      "This is to ensure automatic cleanup and to eliminate chances of dangling references");
        auto [itr, status] =
                callbacks.emplace(new DataEventCallbackImpl(instance, functionPointer));
        notifyCreationObservers(instance, *itr);
        return itr->template asWeakPointer<DataEventCallback>();
    }

    bool hasCallbackCreationObserver() const {
        return !callbackCreationObservers.empty();
    }

    void addCallbackCreationObserver(CallbackCreationObserver& callbackCreationObserver) {
        callbackCreationObservers.emplace_back(std::ref(callbackCreationObserver));
    }

    void removeCallback(Callback::ObservingPointer callback) {
        auto itr = callbacks.find(callback);
        if (itr != callbacks.end()) {
            callbacks.erase(itr);
        } else {
            throw std::runtime_error("Cannot remove callback: The callback requested for removal is not a known Callback");
        }
    }

private:
    void notifyCreationObservers(MeshViewerObject const& instance, common::SharedPointer<Callback> callback) const {
        for (auto& callbackCreationObserver : callbackCreationObservers) {
            callbackCreationObserver.get().notifyCallbackCreated(instance, callback);
        }
    }

private:
    using Callbacks = std::unordered_set<Callback::Pointer, common::SmartPointerHasher, common::SmartPointerComparator>;
    static inline Callbacks callbacks{};

    using CallbackCreationObservers = std::vector<std::reference_wrapper<CallbackCreationObserver>>;
    static inline CallbackCreationObservers callbackCreationObservers;

    friend class CallbackFactoryTester;
};

}
