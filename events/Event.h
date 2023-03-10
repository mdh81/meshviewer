#ifndef MESH_VIEWER_EVENT_H
#define MESH_VIEWER_EVENT_H

#include <iostream>

namespace mv { namespace events {

class Event { 
    public:
        Event(const unsigned id) : m_id(id) {} 
        ~Event() = default;
        unsigned getId() const { return m_id; }
        
        // Copy construction is allowed. This allows an instance of Event to be copy constructed
        // and placed in the memory of lookup data structures that map an event to its callback
        Event(const Event& other) : m_id(other.m_id) {} 
        // Move is strictly not necessary, but doesn't feel correct to support copy construction
        // and leave out move construction
        Event(Event&& other) : m_id(other.m_id) {} 
        // No use case to assign one event to another
        Event& operator=(const Event&) = delete;
        Event& operator=(Event&& other) = delete;

        struct EventEquals {
            bool operator()(const Event& eventA, const Event& eventB) const {
                return eventA.m_id == eventB.m_id;
            }
        };

        struct EventHash {
            size_t operator()(const Event& event) const  {
                return event.m_id;
            }
        };

    protected:
        const unsigned m_id;
};

} }


#endif
