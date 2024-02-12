#pragma once

#include <iostream>
#include "EventTypes.h"

namespace mv::events {

class Event { 
    public:
        Event(unsigned id, unsigned modifier = 0) : id(id), modifier(modifier) {}
        Event(EventId eventId) : id (static_cast<unsigned>(eventId)), modifier(0) {}
        Event(EventId eventId, unsigned modifier) : id (static_cast<unsigned>(eventId)), modifier(modifier) {}
        ~Event() = default;
        unsigned getId() const { return id; }
        unsigned getModifier() const { return modifier; }
        
        // Copy construction is allowed. This allows an instance of Event to be copy constructed
        // and placed in the memory of lookup data structures that map an event to its callback
        Event(Event const& other) : id(other.id), modifier(other.modifier) {}
        // Move is strictly not necessary, but doesn't feel correct to support copy construction
        // and leave out move construction
        Event(Event&& other) : id(other.id), modifier(other.modifier) {}
        // No use case to assign one event to another
        Event& operator=(Event const&) = delete;
        Event& operator=(Event&& other) = delete;

        struct EventComparator {
            bool operator()(Event const& eventA, Event const& eventB) const {
                return eventA.id == eventB.id &&
                       eventA.modifier == eventB.modifier;
            }
        };

        struct EventHasher {
            size_t operator()(Event const& event) const  {
                return event.id;
            }
        };

    protected:
        unsigned const id;
        int const modifier;
};

inline std::ostream& operator <<(std::ostream& os, Event const& event) {
    os << "Event id = " << event.getId() << " modifiers = " << event.getModifier() << std::endl;
    return os;
}

}