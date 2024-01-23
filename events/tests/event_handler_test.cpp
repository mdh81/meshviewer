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
        void simulateKeyPress(int key) { EventHandler::handleKeyPress(nullptr, key, 0, GLFW_PRESS, 0); }
        void simulateMouseButtonPress(int button, int modifiers) { EventHandler::handleMouseEvent(nullptr, button, GLFW_PRESS, modifiers); }
};

} }

class Listener {
    public:
        void handleOKey() { cout << "O was pressed"; }
        void handlePKey() { cout << "P was pressed"; }
        void handleLeftMouse() { cout << "Left mouse was pressed"; }
        void handleMiddleMouseWithShiftDown() { cout << "Middle mouse was pressed with shift down"; }
};


TEST(EventHandler, Callback) {
    EventHandler eh;
    Listener t;
    eh.registerCallback(Event{GLFW_KEY_O},
                        CallbackFactory_Old::getInstance().registerCallback(t, &Listener::handleOKey));
    eh.registerCallback(Event{GLFW_KEY_P},
                        CallbackFactory_Old::getInstance().registerCallback(t, &Listener::handlePKey));
    eh.registerCallback(Event {GLFW_MOUSE_BUTTON_LEFT},
                        CallbackFactory_Old::getInstance().registerCallback(t, &Listener::handleLeftMouse));
    eh.registerCallback(Event {GLFW_MOUSE_BUTTON_MIDDLE, GLFW_MOD_SHIFT},
                        CallbackFactory_Old::getInstance().registerCallback(t, &Listener::handleMiddleMouseWithShiftDown));
    testing::internal::CaptureStdout();
    EventHandlerTest().simulateKeyPress(GLFW_KEY_O);
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "O was pressed");
    testing::internal::CaptureStdout();
    EventHandlerTest().simulateKeyPress(GLFW_KEY_P);
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "P was pressed");
    testing::internal::CaptureStdout();
    EventHandlerTest().simulateMouseButtonPress(GLFW_MOUSE_BUTTON_LEFT, 0);
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "Left mouse was pressed");
    testing::internal::CaptureStdout();
    EventHandlerTest().simulateMouseButtonPress(GLFW_MOUSE_BUTTON_MIDDLE, GLFW_MOD_SHIFT);
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "Middle mouse was pressed with shift down");
}
