#pragma once

#include <cstddef> // for size_t
#include <iostream>

namespace mv {

// An object in this application that is uniquely identified
// by its identifier 
class MeshViewerObject {
    public:
        MeshViewerObject();
        ~MeshViewerObject();
        MeshViewerObject(MeshViewerObject const&);
        MeshViewerObject(MeshViewerObject&&);
        MeshViewerObject& operator=(MeshViewerObject const&);
        MeshViewerObject& operator=(MeshViewerObject&&);

        // Compares object identifiers
        bool operator==(MeshViewerObject const& another) const;

        // Turns debug on or off
        void debugOn() { debug = true; }
        void debugOff() { debug = false; }
        [[nodiscard]] bool isDebugOn() const { return debug; }

        [[nodiscard]] size_t getId() const { return id; }

        [[nodiscard]] bool wasMoved() const { return moved; }

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

    using MeshViewerObjectReference = std::reference_wrapper<MeshViewerObject>;
    using MeshViewerObjectConstReference = std::reference_wrapper<const MeshViewerObject>;

    private:
        size_t id;
        static size_t instanceCount;
        bool moved{};
    protected:
        bool debug{};
};

}