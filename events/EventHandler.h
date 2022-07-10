#ifndef MESH_VIEWER_EVENT_HANDLER_H
#define MESH_VIEWER_EVENT_HANDLER_H

#include "Callback.h" 

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
        EventHandler(EventHandler const&) = delete;
        EventHandler& operator=(EventHandler const&) = delete;
        EventHandler(EventHandler&&) = delete;
        EventHandler& operator==(EventHandler&&) = delete;
        
        
    public:        
        // Register a call back to be called when a key press occurs

};

} }

#endif
