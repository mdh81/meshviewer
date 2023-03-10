#include "EventHandler.h"
#include <iostream>
#include <exception>
#include <unordered_map>

using namespace std;

namespace mv { namespace events {

// init static variables
bool EventHandler::m_started = false;
EventHandler::EventCallbackMap EventHandler::m_eventCallbackMap;

void EventHandler::handleKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS) return;
    auto lookupRes = m_eventCallbackMap.find(Event(key));
    if (lookupRes != m_eventCallbackMap.end()) {
        lookupRes->second.get().call();
    } else {
       cerr << "No callback associated with " << key << ' ' << action << endl; 
    }
}

void EventHandler::registerCallback(const Event& event, const Callback& callback) {
    cerr << "registering " << event.getId() << endl;
    m_eventCallbackMap.emplace(event, std::ref(callback));
}

void EventHandler::start(GLFWwindow* window) {
    if (!m_started) {
        glfwSetKeyCallback(window, &handleKeyPress);
        m_started = true;
    } else {
        runtime_error("Event handler already initialized");
    }
}

} }
