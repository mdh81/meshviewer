#ifndef MESH_VIEWER_OBJECT_H
#define MESH_VIEWER_OBJECT_H

#include <cstddef> // for size_t

namespace meshviewer {

// An object in this application that is uniquely identified
// by its identifier 
class MeshViewerObject {
    public:
        MeshViewerObject();
        bool operator == (const MeshViewerObject& another) const;
        // TODO: Experiment with creation semantics
        void debugOn() { m_debug = true; }
        void debugOff() { m_debug = false; }
    private:
        size_t m_id;
        static size_t sm_instanceCount;
        bool m_debug;
};

}


#endif