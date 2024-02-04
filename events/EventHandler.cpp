#include "EventHandler.h"
#include <iostream>
#include <exception>
#include <unordered_map>
#include <sstream>

namespace mv::events {

namespace {
    auto keyPressHandler = [](GLFWwindow* window, int key, int scancode, int action, int modifiers) {
        EventHandler{}.raiseEvent(Event(key, modifiers));
    };

    auto mouseClickHandler = [](GLFWwindow* window, int button, int action, int modifiers) {
        EventHandler{}.raiseEvent(Event(button, modifiers));
    };
}

void EventHandler::start(GLFWwindow* window) {
    if (!started) {
        glfwSetKeyCallback(window, keyPressHandler);
        glfwSetMouseButtonCallback(window, mouseClickHandler);
        started = true;
    } else {
        std::runtime_error("Event handler already initialized");
    }
}

Callback::ObservingPointer EventHandler::getEventHandler(Event const& event) const {
    Callback::ObservingPointer callback;
    auto lookupRes = callbackMap.find(event);
    if (lookupRes != callbackMap.end()) {
        callback = lookupRes->second;
    }
    return callback;
}

std::string EventHandler::executeBasicEventCallback(Callback::ObservingPointer callback) const {
    bool isCompatible;
    auto basicEventCallback = std::dynamic_pointer_cast<BasicEventCallback>(callback.lock());
    if ((isCompatible = (basicEventCallback != nullptr))) {
        basicEventCallback->call();
    }
    return !isCompatible ? "Basic event callbacks cannot be executed with dynamic data" : std::string{};
}

std::string EventHandler::executeDataEventCallback(Callback::ObservingPointer callback, DynamicEventData&& eventData) const {
    bool isCompatible;
    auto dataEventCallback = std::dynamic_pointer_cast<DataEventCallback>(callback.lock());
    if ((isCompatible = (dataEventCallback != nullptr))) {
        dataEventCallback->call(std::move(eventData));
    }
    return !isCompatible ? "Data event callbacks cannot be executed without event data" : std::string{};
}

std::string EventHandler::executeCallback(Callback::ObservingPointer callback, DynamicEventData&& eventData) const {
    return eventData.empty() ? executeBasicEventCallback(callback) :
                               executeDataEventCallback(callback, std::move(eventData));
}

void EventHandler::raiseEvent(Event const& event, DynamicEventData&& eventData) {
    auto callback = getEventHandler(event);
    if (!callback.expired()) {
        std::string error = executeCallback(callback, std::move(eventData));
        if (!error.empty()) {
            std::stringstream ss;
            ss << event;
            throw std::runtime_error("Incompatible event data " + ss.str() + ' ' + error);
        }
    } else if (isDebugOn()) {
        std::cerr << "No callback associated with " << event << std::endl;
    }
}

}
