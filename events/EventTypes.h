#pragma once
#include <vector>
#include <any>

namespace mv::events {

    enum class EventId : unsigned {
        Scrolled           = 1000,
        FrameBufferResized = 1001,
        CursorMoved        = 1002,
        Zoomed             = 1003,
        Panned             = 1004,
        ScrollRotated      = 1005,
        DragRotated        = 1006,
        DragCompleted      = 1007,
    };

    enum class MouseButton : unsigned {
        Left    = 0,
        Right   = 1
    };

    enum class ButtonAction : unsigned {
        Press = 1,
        Release = 0
    };

    using EventData = std::vector<std::any>;

}
