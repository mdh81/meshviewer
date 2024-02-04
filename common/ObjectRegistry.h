#pragma once

#include "MeshViewerObject.h"
#include <unordered_set>
#include <string>
#include <vector>

namespace mv {

    struct ObjectDeletionObserver {
        virtual void notifyObjectDeleted(mv::MeshViewerObject const&) = 0;
    };

    class ObjectRegistry {

    private:
        void registerObject(MeshViewerObject const& object) {
           registeredObjects.insert(std::ref(object));
        }

        void unRegisterObject(MeshViewerObject const& object) {
            // Remove from registry if the object wasn't moved from
            if (!object.wasMoved()) {
                auto iterator = getIterator(object);
                if (iterator == registeredObjects.end()) {
                    throw std::runtime_error(
                            "Object with id " + std::to_string(object.getId()) + " was not registered");
                }
                registeredObjects.erase(iterator);
            }

            // Notify observers
            for (auto& deletionObserver : deletionObservers) {
                deletionObserver.get().notifyObjectDeleted(object);
            }
        }

    public:
        bool hasRegisteredObjects() {
            return !registeredObjects.empty();
        }

        bool isRegistered(MeshViewerObject const& object) {
            return getIterator(object) != registeredObjects.end();
        }

        void addObjectDeletionObserver(ObjectDeletionObserver& deletionObserver) {
            deletionObservers.emplace_back(std::ref(deletionObserver));
        }

        bool hasDeletionObservers() const {
            return deletionObservers.size() > 0;
        }

    private:
        using RegisteredObjects = std::unordered_set<
                MeshViewerObject::MeshViewerObjectConstReference,
                MeshViewerObject::MeshViewerObjectHash,
                MeshViewerObject::MeshViewerObjectEquals>;
        inline static RegisteredObjects registeredObjects {};

        using DeletionObservers = std::vector<std::reference_wrapper<ObjectDeletionObserver>>;
        inline static DeletionObservers deletionObservers;

        RegisteredObjects::iterator getIterator(MeshViewerObject const& object) {
            return registeredObjects.find(object);
        }

        friend class mv::MeshViewerObject;
    };

}