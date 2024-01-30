#include "gtest/gtest.h"
#include "MeshViewerObject.h"
#include "CallbackFactory.h"
#include <iostream>

TEST(BasicEventCallback, Callback) {
    struct TestClass : mv::MeshViewerObject {
        void dummyCallbackFunction() {
            std::cout << "Dummy callback function called without any arguments" << std::endl;
        }
        void dummyCallbackFunctionOne(int a, char b, double c) {
            std::cout << "Dummy callback function called with " << a << ' ' << b << ' ' << c << std::endl;
        }
    };
    testing::internal::CaptureStdout();
    TestClass testObjectOuter;
    auto basicEventCallback = mv::events::CallbackFactory{}.createBasicEventCallback(testObjectOuter, &TestClass::dummyCallbackFunction);
    basicEventCallback.lock()->call();
    ASSERT_STREQ(testing::internal::GetCapturedStdout().data(), "Dummy callback function called without any arguments\n");

    testing::internal::CaptureStdout();
    auto basicEventCallbackOne = mv::events::CallbackFactory{}.createBasicEventCallback(testObjectOuter, &TestClass::dummyCallbackFunctionOne, 100, 'x', 12.5);
    basicEventCallbackOne.lock()->call();
    ASSERT_STREQ(testing::internal::GetCapturedStdout().data(), "Dummy callback function called with 100 x 12.5\n");
}
