#include "ViewerFactory.h"
#include "ViewerImpl.h"

namespace mv::viewer {
    Viewer& ViewerFactory::getViewer(ui::UserInterface&& ui) {
        static ViewerImpl viewer{ui};
        return viewer;
    }
}
