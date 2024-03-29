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
    };

    using EventData = std::vector<std::any>;

}
