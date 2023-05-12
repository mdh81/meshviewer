#include "EventHandler.h"
#include <iostream>
#include <exception>
#include <unordered_map>

using namespace std;

namespace mv { namespace events {

// init static variables
bool EventHandler::m_started = false;
EventHandler::EventCallbackMap EventHandler::m_eventCallbackMap;

void EventHandler::handleKeyPress(GLFWwindow* window, int key, int scancode, int action, int modifiers) {
    if (action == GLFW_PRESS) {
        handleKeyOrMouseEvent(key, modifiers);
    }
}

void EventHandler::handleMouseEvent(GLFWwindow* window, int button, int action, int modifiers) {
    if (action == GLFW_PRESS) {
        handleKeyOrMouseEvent(button, modifiers);
    }
}

void EventHandler::handleKeyOrMouseEvent(int keyOrButtonIdentifier, int modifiers) {
    auto callback = getEventHandler(keyOrButtonIdentifier, modifiers);
    if (callback) {
        callback->call();
    } else {
        cerr << "No callback associated with " << keyOrButtonIdentifier << " and modifier " << modifiers << endl;
    }
}

void EventHandler::registerCallback(const Event& event, const Callback& callback) {
    m_eventCallbackMap.emplace(event, std::ref(callback));
}

void EventHandler::start(GLFWwindow* window) {
    if (!m_started) {
        glfwSetKeyCallback(window, &handleKeyPress);
        glfwSetMouseButtonCallback(window, &handleMouseEvent);
        m_started = true;
    } else {
        runtime_error("Event handler already initialized");
    }
}

Callback const* EventHandler::getEventHandler(int const glfwKeyOrButton, int const mods) {
    Callback const* callback = nullptr;
    auto lookupRes = m_eventCallbackMap.find(Event(glfwKeyOrButton, mods));
    if (lookupRes != m_eventCallbackMap.end()) {
        callback = &(lookupRes->second.get());
    }
    return callback;
}
} }
