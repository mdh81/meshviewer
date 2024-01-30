#include "gtest/gtest.h"
#include "CallbackManager.h"
#include "ObjectRegistry.h"
#include "MeshViewerObject.h"

TEST(CallbacksManager, Creation) {
    ASSERT_FALSE(mv::ObjectRegistry{}.hasDeletionObservers());
    ASSERT_FALSE(mv::events::CallbackFactory{}.hasCallbackCreationObserver());
    mv::CallbackManager::create();
    ASSERT_TRUE(mv::ObjectRegistry{}.hasDeletionObservers()) <<
    "Callback manager was not added as an object deletion observer" << std::endl;
    ASSERT_TRUE(mv::events::CallbackFactory{}.hasCallbackCreationObserver()) <<
    "Callback manager was not added as a callback creation observer" << std::endl;
}

TEST(CallbacksManager, CallbackTracking) {

    struct TestClass : mv::MeshViewerObject {
        void dummyCallbackFunction() {}
    };
    auto& callbackManager = mv::CallbackManager::create();
    TestClass testObject;
    mv::events::CallbackFactory{}.createBasicEventCallback(
            testObject, &TestClass::dummyCallbackFunction);
    ASSERT_TRUE(callbackManager.hasCallbacksForObject(testObject))
    << "Callback manager is not tracking callbacks for test object";
    ASSERT_EQ(1, callbackManager.getNumberOfRegisteredCallbacks(testObject))
    << "Callback manager is not tracking callbacks for test object";
    mv::events::CallbackFactory{}.createBasicEventCallback(
            testObject, &TestClass::dummyCallbackFunction);
    ASSERT_EQ(2, callbackManager.getNumberOfRegisteredCallbacks(testObject))
    << "Callback manager is not tracking callbacks for test object";

    ASSERT_EQ(0, callbackManager.getNumberOfRegisteredCallbacks(TestClass{}))
    << "Unexpected number of callbacks for an object for which no callbacks were registered";
}

TEST(CallbacksManager, CallbackRemoval) {

    struct TestClass : mv::MeshViewerObject {
        void dummyCallbackFunction() {}
    };

    auto& callbackManager = mv::CallbackManager::create();
    TestClass testObjectOuter;
    mv::events::CallbackFactory{}.createBasicEventCallback(testObjectOuter, &TestClass::dummyCallbackFunction);
    {
        TestClass testObject;
        mv::events::CallbackFactory{}.createBasicEventCallback(testObject, &TestClass::dummyCallbackFunction);
        mv::events::CallbackFactory{}.createBasicEventCallback(testObject, &TestClass::dummyCallbackFunction);
        ASSERT_EQ(2, callbackManager.getNumberOfRegisteredObjects());
    }

    ASSERT_EQ(1, callbackManager.getNumberOfRegisteredObjects())
    << "Callback manager failed to handle deletion of object correctly";
    ASSERT_EQ(1, callbackManager.getNumberOfRegisteredCallbacks(testObjectOuter))
    << "Callback manager unexpectedly removed callbacks for a valid object";
}