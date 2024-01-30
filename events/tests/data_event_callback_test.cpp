#include "gtest/gtest.h"
#include "MeshViewerObject.h"
#include "CallbackFactory.h"
#include <iostream>
#include <any>
#include <vector>

TEST(DataEventCallback, Callback) {
    struct TestClass : mv::MeshViewerObject {
        void dataEventCallback(std::vector<std::any>&& eventData) {
            std::cout << "Data event callback function called with any sequence: " <<
                std::any_cast<int>(eventData[0]) << ' ' << std::any_cast<char>(eventData[1]) << ' ' << std::any_cast<double>(eventData[2]) << std::endl;
        }
    };
    testing::internal::CaptureStdout();
    TestClass testObjectOuter;
    auto dataEventCallback = mv::events::CallbackFactory{}.createDataEventCallback(testObjectOuter,
                                                                                   &TestClass::dataEventCallback);
    dataEventCallback.lock()->call({10, 'x', 12.5});
    ASSERT_STREQ(testing::internal::GetCapturedStdout().data(),
                 "Data event callback function called with any sequence: 10 x 12.5\n");
}
