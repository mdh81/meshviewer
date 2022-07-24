#ifndef MESH_VIEWER_EVENT_HANDLER_H
#define MESH_VIEWER_EVENT_HANDLER_H

#include "Callback.h" 
#include "Event.h"
#include "GLFW/glfw3.h"
#include <unordered_map>
#include <functional>

class GLFWwindow;

namespace meshviewer { namespace events {

class EventHandler {

    public:
        static EventHandler& getInstance() {
            static EventHandler instance;
            return instance;
        }

        // Start listening for events
        void start(GLFWwindow* window);

    private:
        EventHandler() = default; 
        ~EventHandler() = default;
        
        // Enforce singleton
        EventHandler(EventHandler const&) = delete;
        EventHandler& operator=(EventHandler const&) = delete;
        EventHandler(EventHandler&&) = delete;
        EventHandler& operator==(EventHandler&&) = delete;
        
    public:        
        void registerCallback(const Event&, const Callback& callback);

    private:
        using CallbackRef = std::reference_wrapper<const Callback>;
        using EventRef = std::reference_wrapper<const Event>;
        using EventCallbackMap = std::unordered_map<EventRef, CallbackRef, Event::EventHash, Event::EventEquals>;
        EventCallbackMap m_eventCallbackMap;
};

} }

#endif
