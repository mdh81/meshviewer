#include "EventHandler.h"
#include "EventTypes.h"
#include <iostream>
#include <exception>
#include <unordered_map>
#include <sstream>

namespace mv::events {


// NOLINTBEGIN(readability-convert-member-functions-to-static)

namespace {
    auto keyListener = [](GLFWwindow* window, int key, int scancode, int action, int modifiers) {
        EventHandler eventHandler;
        std::unordered_map<unsigned, unsigned> modifierKeyToModifierMap = {{GLFW_KEY_LEFT_CONTROL, GLFW_MOD_CONTROL},
                                                                           {GLFW_KEY_RIGHT_CONTROL, GLFW_MOD_CONTROL},
                                                                           {GLFW_KEY_LEFT_SHIFT, GLFW_MOD_SHIFT},
                                                                           {GLFW_KEY_RIGHT_SHIFT, GLFW_MOD_SHIFT},
                                                                           {GLFW_KEY_LEFT_ALT, GLFW_MOD_ALT},
                                                                           {GLFW_KEY_RIGHT_ALT, GLFW_MOD_ALT}};
        auto itr = modifierKeyToModifierMap.find(key);
        bool isModifierKeyEvent = itr != modifierKeyToModifierMap.end();
        if (action == GLFW_PRESS) {
            isModifierKeyEvent ?
                eventHandler.notifyModifierKeyPressed(itr->second) : eventHandler.raiseEvent(Event(key, modifiers));
        } else if (isModifierKeyEvent /* && action == GLFW_RELEASE */) {
            eventHandler.notifyModifierKeyReleased(itr->second);
        }
    };

    auto mouseClickListener = [](GLFWwindow* window, int button, int action, int modifiers) {
        EventHandler{}.raiseEvent(Event(button, modifiers, action));
    };

    auto frameBufferResizeListener = [](GLFWwindow *window, int width, int height) {
        float xScale{}, yScale{};
        glfwGetWindowContentScale(window, &xScale, &yScale);
        int windowWidth = static_cast<int>(static_cast<float>(width) / xScale);
        int windowHeight = static_cast<int>(static_cast<float>(height) / yScale);
        EventHandler{}.raiseEvent(Event{EventId::FrameBufferResized},
                                  {static_cast<unsigned>(width), static_cast<unsigned>(height),
                                   static_cast<unsigned>(windowWidth), static_cast<unsigned>(windowHeight)});
    };

    auto scrollListener = [](GLFWwindow* window, double xOffset, double yOffset) {
        unsigned modifierKeys {};
        EventHandler eventHandler;
        if (eventHandler.isModifierKeyPressed(GLFW_MOD_SHIFT)) {
            modifierKeys |= GLFW_MOD_SHIFT;
        }
        if (eventHandler.isModifierKeyPressed(GLFW_MOD_CONTROL)) {
            modifierKeys |= GLFW_MOD_CONTROL;
        }
        eventHandler.raiseEvent(Event{EventId::Scrolled, modifierKeys}, {static_cast<float>(xOffset),
                                                                         static_cast<float>(yOffset),
                                                                         modifierKeys});
    };

    auto cursorPositionListener = [](GLFWwindow* window, double x, double y) {
        // glfwSetCursorPosCallback is triggered even when the cursor is outside the window. Only handle cursor events
        // that happen within the window
        int width{}, height{};
        glfwGetWindowSize(window, &width, &height);
        if (x > 0 && y > 0 && x < width && y < height) {
            EventHandler{}.raiseEvent(Event{EventId::CursorMoved}, {static_cast<float>(x), static_cast<float>(y)});
        }
    };
}

void EventHandler::start(GLFWwindow* window) {
    if (!started) {
        glfwSetKeyCallback(window, keyListener);
        glfwSetMouseButtonCallback(window, mouseClickListener);
        glfwSetFramebufferSizeCallback(window, frameBufferResizeListener);
        glfwSetScrollCallback(window, scrollListener);
        glfwSetCursorPosCallback(window, cursorPositionListener);
        started = true;
        return;
    }
    throw std::runtime_error("Event handler already initialized");
}

Callback::ObservingPointer EventHandler::getEventHandler(Event const& event) const {
    Callback::ObservingPointer callback;
    auto lookupRes = callbackMap.find(event);
    if (lookupRes != callbackMap.end()) {
        callback = lookupRes->second;
    }
    return callback;
}

void EventHandler::executeBasicEventCallback(Callback::ObservingPointer callback) const {
    bool isCompatible;
    auto basicEventCallback = std::dynamic_pointer_cast<BasicEventCallback>(callback.lock());
    if ((isCompatible = (basicEventCallback != nullptr))) {
        basicEventCallback->call();
    }
    if (!isCompatible) {
        throw std::runtime_error("Basic event callbacks cannot be executed with dynamic data");
    }
}

void EventHandler::executeDataEventCallback(Callback::ObservingPointer callback, EventData&& eventData) const {
    bool isCompatible;
    auto dataEventCallback = std::dynamic_pointer_cast<DataEventCallback>(callback.lock());
    if ((isCompatible = (dataEventCallback != nullptr))) {
        dataEventCallback->call(std::move(eventData));
    }
    if (!isCompatible) {
        throw std::runtime_error("Data event callbacks cannot be executed without event data");
    }
}

void EventHandler::executeCallback(Callback::ObservingPointer callback, EventData&& eventData) const {
    return eventData.empty() ? executeBasicEventCallback(callback) :
                               executeDataEventCallback(callback, std::move(eventData));
}

void EventHandler::raiseEvent(Event const& event, EventData&& eventData) const {
    // Call event handler
    auto callback = getEventHandler(event);
    if (!callback.expired()) {
        executeCallback(callback, std::move(eventData));
        // Call post-event handler
        auto postEventCallback = getEventHandler(EventId::EventProcessingCompleted);
        if (!postEventCallback.expired()) {
            executeDataEventCallback(postEventCallback, {event});
        }
    } else if (isDebugOn()) {
        std::cerr << "No callback associated with " << event << std::endl;
    }
}

// Check if modifier key is currently pressed
bool EventHandler::isModifierKeyPressed(unsigned modifierKey) const {
    return modifierKeys & modifierKey;
}

void EventHandler::notifyModifierKeyPressed(unsigned modifierKey) {
    modifierKeys |= modifierKey;
}

void EventHandler::notifyModifierKeyReleased(unsigned modifierKey) {
    modifierKeys &= ~modifierKey;
}

// NOLINTEND(readability-convert-member-functions-to-static)

}
