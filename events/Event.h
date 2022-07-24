#ifndef MESH_VIEWER_EVENT_H
#define MESH_VIEWER_EVENT_H

namespace meshviewer { namespace events {

class Event { 
    public:
        Event(const unsigned id) : m_id(id) {} 
        ~Event() = default;
        // Events are distinct objects. Prevent copies
        // and assignment
        Event(const Event&) = delete;
        Event& operator=(const Event&) = delete;
        Event(Event&&) = delete;
        Event& operator=(Event&&) = delete;

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

class KeyEvent : public Event {
    public:
        enum class Trigger {
            Press,
            Release
        };

        KeyEvent(const unsigned id, const Trigger trigger) 
            : Event(id)
            , m_trigger(trigger) {

        }
        
        // Events are distinct objects. Prevent copies
        // and assignment
        KeyEvent(const KeyEvent&) = delete;
        KeyEvent& operator=(const KeyEvent&) = delete;
        KeyEvent(KeyEvent&&) = delete;
        KeyEvent& operator=(KeyEvent&&) = delete;

    private:
        Trigger m_trigger;
};

class MouseEvent : public Event {

};


} }


#endif
