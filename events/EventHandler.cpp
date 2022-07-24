#include "EventHandler.h"
#include <iostream>
#include <unordered_map>

using namespace std;

namespace meshviewer { namespace events {

namespace {

void handleKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS)
        cout << "Key Pressed" << endl;
    if (action == GLFW_RELEASE)
        cout << "Key Released" << endl;
}

}

void EventHandler::registerCallback(const Event& event, const Callback& callback) {
    m_eventCallbackMap.emplace(std::ref(event), std::ref(callback));
}

void EventHandler::start(GLFWwindow* window) {
    glfwSetKeyCallback(window, &handleKeyPress);
}

} }
