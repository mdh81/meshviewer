#ifndef MESH_VIEWER_OBJECT_H
#define MESH_VIEWER_OBJECT_H

#include <cstddef> // for size_t
#include <iostream>

namespace mv {

// An object in this application that is uniquely identified
// by its identifier 
class MeshViewerObject {
    public:
        // TODO: Experiment with creation semantics
        explicit MeshViewerObject(std::ostream& os=std::cerr);
        // Compares object identifiers
        bool operator==(const MeshViewerObject& another) const;
        // Turns debug on or off
        void debugOn() { std::cout<<"Turning debug on" << std::endl; m_debugOn = true; }
        void debugOff() { m_debugOn = false; }
        [[nodiscard]] bool isDebugOn() const { return m_debugOn; }
        [[nodiscard]] size_t getId() const { return m_id; }

        struct MeshViewerObjectHash {
            size_t operator()(MeshViewerObject const& meshViewerObject) const {
                return meshViewerObject.getId();
            }
        };

        struct MeshViewerObjectEquals {
            bool operator()(MeshViewerObject const& meshViewerObjectA,
                            MeshViewerObject const& meshViewerObjectB) const {
                return meshViewerObjectA.getId() == meshViewerObjectB.getId();
            }
        };


    private:
        size_t m_id;
        static size_t sm_instanceCount;
    protected:
        bool m_debugOn;
        std::ostream& m_outputStream; 
};

}


#endif
