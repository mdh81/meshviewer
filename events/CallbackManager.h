#pragma once

#include "ObjectRegistry.h"
#include "Callback.h"
#include "MeshViewerObject.h"
#include "CallbackFactory.h"

// CallbacksManager ensures the validity of callbacks in the system by ensuring they don't outlive the object that provides
// definition for the callback

namespace mv {
    class CallbackManager : public ObjectDeletionObserver, CallbackCreationObserver {
    public:
        // NOTE: nodiscard omitted to give the app code ability to create anonymous instances
        static CallbackManager& create() {
            static CallbackManager instance;
            return instance;
        }

        ~CallbackManager() = default;

        void notifyObjectDeleted(MeshViewerObject const& object) override;

        void notifyCallbackCreated(MeshViewerObject const& object,
                                   Callback::ObservingPointer callback) override;

        bool hasCallbacksForObject(MeshViewerObject const& object) const;

        unsigned getNumberOfRegisteredCallbacks(MeshViewerObject const& object) const;

        unsigned getNumberOfRegisteredObjects() const;

    private:
        CallbackManager();

        // Disallow copy construction and assignment
        CallbackManager(CallbackManager const&) = delete;
        CallbackManager& operator=(CallbackManager const&) = delete;

        // Disallow move construction and assignment
        CallbackManager(CallbackManager&&) = delete;
        CallbackManager& operator=(CallbackManager &&) = delete;

    private:
        using CallbackPointers = std::vector<Callback::ObservingPointer>;

        using ObjectCallbackMap =
                std::unordered_map<MeshViewerObject::MeshViewerObjectConstReference, CallbackPointers,
                        MeshViewerObject::MeshViewerObjectHash, MeshViewerObject::MeshViewerObjectEquals>;
        ObjectCallbackMap objectCallbackMap{};
    };
}