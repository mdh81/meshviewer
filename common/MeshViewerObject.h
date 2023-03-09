#ifndef MESH_VIEWER_OBJECT_H
#define MESH_VIEWER_OBJECT_H

#include <cstddef> // for size_t
#include <iostream>

namespace meshviewer {

// An object in this application that is uniquely identified
// by its identifier 
class MeshViewerObject {
    public:
        // TODO: Experiment with creation semantics
        MeshViewerObject(std::ostream& os=std::cerr);
        // Compares object identifiers
        bool operator==(const MeshViewerObject& another) const;
        // Turns debug on or off
        void debugOn() { std::cout<<"Turning debug on" << std::endl; m_debugOn = true; }
        void debugOff() { m_debugOn = false; }
        bool isDebugOn() { return m_debugOn; }
        void setOutput(std::ostream& os);
        size_t getId() { return m_id; }
    private:
        size_t m_id;
        static size_t sm_instanceCount;
    protected:
        bool m_debugOn;
        std::ostream& m_outputStream; 
};

}


#endif
