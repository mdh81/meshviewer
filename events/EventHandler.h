#ifndef MESH_VIEWER_EVENT_HANDLER_H
#define MESH_VIEWER_EVENT_HANDLER_H

#include "Callback.h" 
#include "Event.h"
#include "GLFW/glfw3.h"
#include <unordered_map>
#include <functional>

class GLFWwindow;

namespace meshviewer { namespace events {

// A monostate class that is responsible for handling events. Events are handled in the context
// of a GLFW window.

class EventHandler {

    public:
        EventHandler() = default;
        ~EventHandler() = default;

        // Start listening for events in the specified window
        void start(GLFWwindow* window);
        
        // Register a callback for the specified event 
        void registerCallback(const Event&, const Callback& callback);
        
        // Disallow copies as copying is not a meaningful operation
        // for monostate instances 
        EventHandler(EventHandler const&) = delete;
        EventHandler& operator=(EventHandler const&) = delete;
        EventHandler(EventHandler&&) = delete;
        EventHandler& operator==(EventHandler&&) = delete;
        
    private:
        // This method is declared static so a regular function pointer can be created from this
        // method and passed to glfw function. These old C APIs don't support member function pointers
        static void handleKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods);

    private:
        using CallbackRef = std::reference_wrapper<const Callback>;
        using EventCallbackMap = std::unordered_map<const Event, CallbackRef, Event::EventHash, Event::EventEquals>;
        static EventCallbackMap m_eventCallbackMap;
        static bool m_started;

    friend class EventHandlerTest;
};

} }

#endif
