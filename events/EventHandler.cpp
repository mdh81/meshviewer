#include "EventHandler.h"
#include <iostream>
#include <exception>
#include <unordered_map>

namespace mv::events {

// init static variables
bool EventHandler::started = false;
EventHandler::EventCallbackMap EventHandler::eventCallbackMap;
unsigned EventHandler::modifierKeys = 0;
MeshViewerObject* EventHandler::self = nullptr;

void EventHandler::handleKeyPress(GLFWwindow* window, int key, int scancode, int action, int modifiers) {
    if (action == GLFW_PRESS) {
        handleKeyOrMouseEvent(key, modifiers);
    }
    modifierKeys = modifiers;
}

void EventHandler::handleMouseEvent(GLFWwindow* window, int button, int action, int modifiers) {
    if (action == GLFW_PRESS) {
        handleKeyOrMouseEvent(button, modifiers);
    }
    modifierKeys = modifiers;
}

void EventHandler::handleKeyOrMouseEvent(int keyOrButtonIdentifier, int modifiers) {
    auto callback = getEventHandler(keyOrButtonIdentifier, modifiers);
    if (callback) {
        callback->call();
    } else if (self->isDebugOn()){
        std::cerr << "No callback associated with " << keyOrButtonIdentifier << " and modifier " << modifiers << std::endl;
    }
}

void EventHandler::registerCallback(const Event& event, const Callback_Old& callback) {
    eventCallbackMap.emplace(event, std::ref(callback));
}

void EventHandler::start(GLFWwindow* window) {
    if (!started) {
        glfwSetKeyCallback(window, &handleKeyPress);
        glfwSetMouseButtonCallback(window, &handleMouseEvent);
        started = true;
        self = this;
    } else {
        std::runtime_error("Event handler already initialized");
    }
}

Callback_Old const* EventHandler::getEventHandler(int const glfwKeyOrButton, int const mods) {
    Callback_Old const* callback = nullptr;
    auto lookupRes = eventCallbackMap.find(Event(glfwKeyOrButton, mods));
    if (lookupRes != eventCallbackMap.end()) {
        callback = &(lookupRes->second.get());
    }
    return callback;
}

void EventHandler::raiseEvent(const Event &event) {
    handleKeyOrMouseEvent(event.getId(), event.getModifier());
}

}
