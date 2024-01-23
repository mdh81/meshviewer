#ifndef MESH_VIEWER_EVENT_H
#define MESH_VIEWER_EVENT_H

#include <iostream>

namespace mv { namespace events {

class Event { 
    public:
        Event(const unsigned id, const unsigned modifier = 0) : m_id(id), m_modifier(modifier) {}
        ~Event() = default;
        unsigned getId() const { return m_id; }
        unsigned getModifier() const { return m_modifier; }
        
        // Copy construction is allowed. This allows an instance of Event to be copy constructed
        // and placed in the memory of lookup data structures that map an event to its callback
        Event(Event const& other) : m_id(other.m_id), m_modifier(other.m_modifier) {}
        // Move is strictly not necessary, but doesn't feel correct to support copy construction
        // and leave out move construction
        Event(Event&& other) : m_id(other.m_id), m_modifier(other.m_modifier) {}
        // No use case to assign one event to another
        Event& operator=(Event const&) = delete;
        Event& operator=(Event&& other) = delete;

        struct EventComparator {
            bool operator()(Event const& eventA, Event const& eventB) const {
                return eventA.m_id == eventB.m_id &&
                    eventA.m_modifier == eventB.m_modifier;
            }
        };

        struct EventHasher {
            size_t operator()(Event const& event) const  {
                return event.m_id;
            }
        };

    protected:
        const unsigned m_id;
        const int m_modifier;
};

} }


#endif
