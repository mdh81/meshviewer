#pragma once

#include "UserInterface.h"
#include "gmock/gmock.h"

namespace mv::ui {
    class MockUserInterface final : public UserInterface {
        MOCK_METHOD(void, beginDraw, (GLFWwindow*), (override));
        MOCK_METHOD(void, endDraw, (), (override));
        MOCK_METHOD(void, stop, (), (override));
    };
};
