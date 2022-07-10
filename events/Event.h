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
        Event(const Event&) = delete;
        Event& operator=(const Event&) = delete;
        Event(Event&&) = delete;
        Event& operator=(Event&&) = delete;

    private:
        Trigger m_trigger;


};

class MouseEvent : public Event {

};


} }


#endif
