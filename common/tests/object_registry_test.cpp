#include "gtest/gtest.h"
#include "MeshViewerObject.h"
#include "ObjectRegistry.h"

TEST(ObjectRegistry, AutomaticRegistrationDeregistration) {
    {
        mv::MeshViewerObject objectOne;
        ASSERT_TRUE(mv::ObjectRegistry{}.isRegistered(objectOne));
    }
    ASSERT_FALSE(mv::ObjectRegistry{}.hasRegisteredObjects());
}

TEST(ObjectRegistry, DeletionObserver) {
    testing::internal::CaptureStdout();
    struct DeletionObserver : mv::ObjectDeletionObserver {
        void notifyObjectDeleted(const mv::MeshViewerObject &object) override {
            std::cout << "Object with id " << object.getId() << " was deleted";
        }
    };
    DeletionObserver deletionObserver;
    mv::ObjectRegistry{}.addObjectDeletionObserver(deletionObserver);
    size_t id{};
    {
        mv::MeshViewerObject objectOne;
        id = objectOne.getId();
    }
    auto str = testing::internal::GetCapturedStdout();
    ASSERT_EQ(str, "Object with id " + std::to_string(id) + " was deleted");
}
