#ifndef MESH_VIEWER_OBJECT_H
#define MESH_VIEWER_OBJECT_H

#include <cstddef> // for size_t

namespace meshviewer {

// An object in this application that is uniquely identified
// by its identifier 
class MeshViewerObject {
    public:
        // TODO: Experiment with creation semantics
        MeshViewerObject();
        // Compares object identifiers
        bool operator==(const MeshViewerObject& another) const;
        // Turns debug on or off
        void debugOn() { m_debugOn = true; }
        void debugOff() { m_debugOn = false; }
        bool isDebugOn() { return m_debugOn; }
        size_t getId() { return m_id; }
    private:
        size_t m_id;
        static size_t sm_instanceCount;
    protected:
        bool m_debugOn;
};

}


#endif
