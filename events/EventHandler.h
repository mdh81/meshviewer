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

namespace mv {

    namespace viewer {
        class ViewerTest;
    }

    namespace events {

    // A monostate class that is responsible for handling events. Events are handled in the context
    // of a GLFW window.
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

            void raiseEvent(Event const& event, std::vector<std::any>&& eventData = {});

            // Check if modifier key is currently pressed
            bool isModifierKeyPressed(unsigned modifierKey) const {
                return modifierKeys & modifierKey;
            }

            void notifyModifierKeyPressed(unsigned modifierKey) {
                modifierKeys |= modifierKey;
            }

            void notifyModifierKeyReleased(unsigned modifierKey) {
                modifierKeys &= ~modifierKey;
            }

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
            inline static int modifierKeys{};


            // Get callback for the specified event
            Callback::ObservingPointer getEventHandler(Event const&) const;
            // Execute callbacks
            std::string executeCallback(Callback::ObservingPointer, EventData&&) const;
            std::string executeBasicEventCallback(Callback::ObservingPointer) const;
            std::string executeDataEventCallback(Callback::ObservingPointer, EventData&&) const;

        friend class EventHandlerTest;
        friend class mv::viewer::ViewerTest;
    };

} }
