#pragma once

#include "Viewer.h"

namespace mv::viewer {
    struct ViewerFactory {
        Viewer& getViewer();
    };
}