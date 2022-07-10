#include "EventHandler.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <unordered_map>

using namespace std;

namespace meshviewer { namespace events {

namespace {

void handleKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS)
        cout << "Key Pressed" << endl;
}

}

void EventHandler::start(GLFWwindow* window) {
    glfwSetKeyCallback(window, &handleKeyPress);
}

} }
