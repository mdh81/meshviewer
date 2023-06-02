#ifndef MESH_VIEWER_EVENT_HANDLER_H
#define MESH_VIEWER_EVENT_HANDLER_H

#include "Callback.h" 
#include "Event.h"
#include "GLFW/glfw3.h"
#include <unordered_map>
#include <functional>

class GLFWwindow;

//TODO: Unify namespace name. mv and meshviewer are used interchangeably in a few places
namespace mv {

    namespace viewer {
        class ViewerTest;
    }

    namespace events {

    // A monostate class that is responsible for handling events. Events are handled in the context
    // of a GLFW window.
    // TODO: Make it inherit from MeshViewerObject so it can turn on debug output similar to other objects
    class EventHandler {

        public:
            EventHandler() = default;
            ~EventHandler() = default;

            // Start listening for events in the specified window
            void start(GLFWwindow* window);

            // Register a callback for the specified event
            void registerCallback(Event const&, Callback const& callback);

            // Raise an event
            void raiseEvent(Event const& event);

            // Check if modifier key is currently pressed
            bool isModifierKeyPressed(unsigned int modifierKey) {
                return modifierKeys & modifierKey;
            }

            // Disallow copies as copying is not a meaningful operation
            // for monostate instances
            EventHandler(EventHandler const&) = delete;
            EventHandler& operator=(EventHandler const&) = delete;
            EventHandler(EventHandler&&) = delete;
            EventHandler& operator==(EventHandler&&) = delete;

        private:
            using CallbackRef = std::reference_wrapper<const Callback>;
            using EventCallbackMap = std::unordered_map<const Event, CallbackRef, Event::EventHash, Event::EventEquals>;
            static EventCallbackMap eventCallbackMap;
            static bool started;
            static unsigned modifierKeys;

            // These methods are declared static so a regular function pointer can be created from these
            // methods and passed to glfw functions. These old C APIs don't support member function pointers
            static void handleKeyPress(GLFWwindow* window, int key, int scancode, int action, int modifiers);
            static void handleMouseEvent(GLFWwindow* window, int button, int action, int modifiers);

            // Handle mouse or keyboard event
            static void handleKeyOrMouseEvent(int keyOrButtonIdentifier, int modifiers);

            // Find the Event object to handle the GLFW mouse or keyboard event
            static Callback const* getEventHandler(int eventIdentifier, int mods);

        friend class EventHandlerTest;
        friend class mv::viewer::ViewerTest;
    };

} }

#endif
