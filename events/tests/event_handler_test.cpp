#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "gtest/gtest.h"
#include "EventHandler.h"
#include "CallbackFactory_Old.h"
#include <string>
#include <iostream>

using namespace mv::events;
using namespace std;

// Friend class implementation that tests internals of EventHandler
namespace mv { namespace events {

class EventHandlerTest {
    public:
        void simulateKeyPress(int key) {
            EventHandler{}.raiseEvent(key);
        }
        void simulateMouseButtonPress(int button, int modifiers) {
            if (modifiers == -1) {
                EventHandler{}.raiseEvent(button);
            } else {
                EventHandler{}.raiseEvent(Event{(unsigned) button, (unsigned) modifiers}, {"dummy", "data"});
            }
        }
        auto getNumCallbacks() {
            return EventHandler::callbackMap.size();
        }
};

} }

class CallbackImplementer : public mv::MeshViewerObject {
    public:
        void handleOKey() { cout << "O was pressed"; }
        void handlePKey() { cout << "P was pressed"; }
        void handleLeftMouse() { cout << "Left mouse was pressed"; }
        void handleMiddleMouseWithShiftDown(std::vector<std::any>&& eventData) { cout << "Middle mouse was pressed with shift down"; }
};

TEST(EventHandler, CallbackRegistration) {
    EventHandlerTest tester;
    EventHandler eh;
    CallbackImplementer t;

    ASSERT_TRUE(tester.getNumCallbacks() == 0) << "Invalid number of callbacks upon initialization";

    eh.registerBasicEventCallback(GLFW_KEY_O, t, &CallbackImplementer::handleOKey);
    ASSERT_TRUE(tester.getNumCallbacks() == 1) << "Invalid number of callbacks after first registration";

    eh.registerBasicEventCallback(GLFW_KEY_O, t, &CallbackImplementer::handleOKey);
    ASSERT_TRUE(tester.getNumCallbacks() == 1) << "Invalid number of callbacks after first registration";

    eh.registerDataEventCallback(GLFW_KEY_1, t, &CallbackImplementer::handleMiddleMouseWithShiftDown);
    ASSERT_TRUE(tester.getNumCallbacks() == 2) << "Invalid number of callbacks after registering data event callback";
}


TEST(EventHandler, RaiseEvent) {
    EventHandler eh;
    CallbackImplementer t;
    eh.registerBasicEventCallback(GLFW_KEY_O, t, &CallbackImplementer::handleOKey);
    eh.registerBasicEventCallback(GLFW_KEY_P, t, &CallbackImplementer::handlePKey);
    eh.registerBasicEventCallback(GLFW_MOUSE_BUTTON_LEFT, t, &CallbackImplementer::handleLeftMouse);
    eh.registerDataEventCallback(Event{GLFW_MOUSE_BUTTON_MIDDLE, GLFW_MOD_SHIFT}, t, &CallbackImplementer::handleMiddleMouseWithShiftDown);
    testing::internal::CaptureStdout();
    EventHandlerTest().simulateKeyPress(GLFW_KEY_O);
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "O was pressed");
    testing::internal::CaptureStdout();
    EventHandlerTest().simulateKeyPress(GLFW_KEY_P);
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "P was pressed");
    testing::internal::CaptureStdout();
    EventHandlerTest().simulateMouseButtonPress(GLFW_MOUSE_BUTTON_LEFT, -1);
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "Left mouse was pressed");
    testing::internal::CaptureStdout();
    EventHandlerTest().simulateMouseButtonPress(GLFW_MOUSE_BUTTON_MIDDLE, GLFW_MOD_SHIFT);
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "Middle mouse was pressed with shift down");
}
