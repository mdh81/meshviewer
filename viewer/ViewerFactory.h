#pragma once

#include "Viewer.h"
#include "../ui/UserInterfaceImpl.h"

namespace mv::viewer {
    struct ViewerFactory {
        static Viewer& getViewer(ui::UserInterface&& = ui::UserInterfaceImpl{});
    };
}
