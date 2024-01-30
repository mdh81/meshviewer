#include "gtest/gtest.h"
#include "Callback.h"
#include "CallbackFactory.h"

namespace mv::events {
    class CallbackFactoryTester {
    public:
        auto getNumCallbacks() {
            return mv::events::CallbackFactory{}.callbacks.size();
        }
    };
}

struct TestClass : public mv::MeshViewerObject {
    void basicEventCallback(int a, float b, unsigned c) {
    }

    void dataEventCallback(std::vector<std::any>&& eventData) {
    }

};

TEST(CallbackFactory, BasicEventCallbackCreation) {
    TestClass testObject;
    mv::events::CallbackFactory{}.createBasicEventCallback(testObject, &TestClass::basicEventCallback, 10, 12.5, 100);
    mv::events::CallbackFactoryTester tester;
    ASSERT_EQ(tester.getNumCallbacks(), 1);
}

TEST(CallbackFactory, DataEventCallbackCreation) {
    TestClass testObject;
    mv::events::CallbackFactory{}.createDataEventCallback(testObject, &TestClass::dataEventCallback);
    mv::events::CallbackFactoryTester tester;
    ASSERT_EQ(tester.getNumCallbacks(), 1);
}

TEST(CallbackFactory, BasicEventCallbackRemoval) {
    TestClass testObject;
    auto callbackPointer = mv::events::CallbackFactory{}.createBasicEventCallback(testObject,
                                                                                  &TestClass::basicEventCallback, 10,
                                                                                  12.5, 100);
    mv::events::CallbackFactory{}.removeCallback(callbackPointer);
    mv::events::CallbackFactoryTester tester;
    ASSERT_EQ(tester.getNumCallbacks(), 0);
}

TEST(CallbackFactory, DataEventCallbackRemoval) {
    TestClass testObject;
    auto callbackPointer = mv::events::CallbackFactory{}.createDataEventCallback(testObject, &TestClass::dataEventCallback);
    mv::events::CallbackFactory{}.removeCallback(callbackPointer);
    mv::events::CallbackFactoryTester tester;
    ASSERT_EQ(tester.getNumCallbacks(), 0);
}