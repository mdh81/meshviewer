#include "CallbackManager.h"

namespace mv {
    CallbackManager::CallbackManager() {
        events::CallbackFactory{}.addCallbackCreationObserver(*this);
        ObjectRegistry{}.addObjectDeletionObserver(*this);
    }

    void CallbackManager::notifyObjectDeleted(MeshViewerObject const& object) {
        auto mapItr = objectCallbackMap.find(object);
        if (mapItr != objectCallbackMap.end()) {
            auto &callbacks = mapItr->second;
            for (auto &callbackObservingPointer: callbacks) {
                if (!callbackObservingPointer.expired()) {
                    events::CallbackFactory{}.removeCallback(callbackObservingPointer);
                } else {
                    throw std::runtime_error("Logic error: Unexpected callbacks state");
                }
            }
            objectCallbackMap.erase(mapItr);
        }
    }

    void CallbackManager::notifyCallbackCreated(MeshViewerObject const& object,
                                                Callback::ObservingPointer callback) {
        auto [itr, result] = objectCallbackMap.emplace(std::ref(object), CallbackPointers{});
        itr->second.emplace_back(callback);
    }

    bool CallbackManager::hasCallbacksForObject(MeshViewerObject const& object) const {
        return objectCallbackMap.find(object) != objectCallbackMap.end();
    }

    unsigned CallbackManager::getNumberOfRegisteredCallbacks(MeshViewerObject const& object) const {
        if (!hasCallbacksForObject(object)) return 0;
        return static_cast<unsigned> (objectCallbackMap.find(object)->second.size());
    }

    unsigned CallbackManager::getNumberOfRegisteredObjects() const {
        return static_cast<unsigned> (objectCallbackMap.size());
    }

}
