#pragma once

#include "Callbacks.h"
#include "CallbackFactory.h"
#include "Event.h"
#include "EventTypes.h"
#include "MeshViewerObject.h"
#include "Types.h"
#include "GLFW/glfw3.h"
#include <unordered_map>
#include <vector>
#include <any>

class GLFWwindow;

namespace mv::events {

// A monostate class that is responsible for handling events. Events are handled in the context
// of a GLFW window.

// NOLINTBEGIN(readability-convert-member-functions-to-static)
    class EventHandler : public MeshViewerObject {

    public:
        EventHandler() = default;
        ~EventHandler() = default;

        // Start listening for events in the specified window
        void start(GLFWwindow* window);

        template<typename InstanceType, typename FunctionPointerType, typename... ArgumentTypes>
        void registerBasicEventCallback(Event&& event, InstanceType& instance, FunctionPointerType functionPointer,
                                        ArgumentTypes&&... callbackArguments) {
            callbackMap.emplace(std::move(event),
                                CallbackFactory{}.
                                createBasicEventCallback(instance,
                                                        functionPointer,
                                                        std::forward<ArgumentTypes>(callbackArguments)...));
        }

        template<typename InstanceType, typename FunctionPointerType>
        void registerDataEventCallback(Event&& event, InstanceType& instance, FunctionPointerType functionPointer) {
            callbackMap.emplace(std::move(event),
                                CallbackFactory{}.createDataEventCallback(instance, functionPointer));
        }

        void raiseEvent(Event const& event, std::vector<std::any>&& eventData = {}) const;

        // Check if modifier key is currently pressed
        [[nodiscard]] bool isModifierKeyPressed(unsigned modifierKey) const;

        void notifyModifierKeyPressed(unsigned modifierKey);

        void notifyModifierKeyReleased(unsigned modifierKey);

        // Disallow copies as copying is not a meaningful operation
        // for monostate instances
        EventHandler(EventHandler const&) = delete;
        EventHandler& operator=(EventHandler const&) = delete;
        EventHandler(EventHandler&&) = delete;
        EventHandler& operator==(EventHandler&&) = delete;

    private:
        using EventCallbackMap =
                std::unordered_map<const Event, Callback::ObservingPointer,
                                   Event::EventHasher, Event::EventComparator>;
        inline static EventCallbackMap callbackMap{};
        inline static bool started{};
        inline static unsigned modifierKeys{};


        // Get callback for the specified event
        [[nodiscard]] Callback::ObservingPointer getEventHandler(Event const&) const;
        // Execute callbacks
        void executeCallback(Callback::ObservingPointer, EventData&&) const;
        void executeBasicEventCallback(Callback::ObservingPointer) const;
        void executeDataEventCallback(Callback::ObservingPointer, EventData&&) const;

    friend class EventHandlerTest;
};
// NOLINTEND(readability-convert-member-functions-to-static)

}
