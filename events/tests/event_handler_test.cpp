#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "gtest/gtest.h"
#include "EventHandler.h"
#include "CallbackFactory.h"
#include <string>
#include <iostream>

using namespace mv::events;
using namespace std;

// Friend class implementation that tests internals of EventHandler
namespace mv { namespace events {

class EventHandlerTest {
    public:
        void simulateKeyPress(int key) { EventHandler::handleKeyPress(nullptr, key, 0, GLFW_PRESS, 0); }
};

} }

class Listener {
    public:
        void handleOKey() { cout << "O was pressed"; }
        void handlePKey() { cout << "P was pressed"; }
};


TEST(EventHandler, Callback) {
    EventHandler eh;
    Listener t;
    eh.registerCallback(Event{GLFW_KEY_O},
                        CallbackFactory::getInstance().registerCallback(t, &Listener::handleOKey)); 
    eh.registerCallback(Event{GLFW_KEY_P},
                        CallbackFactory::getInstance().registerCallback(t, &Listener::handlePKey));
    testing::internal::CaptureStdout();
    EventHandlerTest().simulateKeyPress(GLFW_KEY_O);
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "O was pressed");
    testing::internal::CaptureStdout();
    EventHandlerTest().simulateKeyPress(GLFW_KEY_P);
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "P was pressed");
}
