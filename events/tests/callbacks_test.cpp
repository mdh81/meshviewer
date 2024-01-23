#include "gtest/gtest.h"
#include "Callbacks.h"
#include "CallbackFactory.h"

namespace mv::events {
    class CallbackFactoryTester {
    public:
        auto getNumBasicEventCallbacks() {
            return mv::events::CallbackFactory{}.basicEventCallbacks.size();
        }
    };
}

struct TestClass {
    void basicEventCallback(int a, float b, unsigned c) {

    }

    void dataEventCallback(std::vector<std::any>&& eventData) {

    }

};

TEST(CallbackFactory, BasicEventCallbackCreation) {
    TestClass testObject;
    mv::events::CallbackFactory{}.createCallback(testObject, &TestClass::basicEventCallback, 10, 12.5, 100);
    mv::events::CallbackFactoryTester tester;
    ASSERT_EQ(tester.getNumBasicEventCallbacks(), 1);
}

TEST(CallbackFactory, BasicEventCallbackRemoval) {
    TestClass testObject;
    auto& callbackPointer = mv::events::CallbackFactory{}.createCallback(testObject, &TestClass::basicEventCallback, 10, 12.5, 100);
    mv::events::CallbackFactory{}.removeCallback(callbackPointer);
    mv::events::CallbackFactoryTester tester;
    ASSERT_EQ(tester.getNumBasicEventCallbacks(), 0);
}

