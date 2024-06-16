#include "ViewerFactory.h"
#include "ViewerImpl.h"

namespace mv::viewer {
    Viewer& ViewerFactory::getViewer() {
        static ViewerImpl viewer;
        return viewer;
    }
}
