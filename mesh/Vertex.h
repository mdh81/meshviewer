#ifndef MESH_VIEWER_VERTEX_H
#define MESH_VIEWER_VERTEX_H

#include "Types.h"
#include "Util.h"
#include "3dmath/Vector.h"
#include <vector>

namespace mv {

class Mesh;

class Vertex : public common::Point3D {

    public:
        Vertex() = default;

        Vertex(float const x, float const y, float const z)
        : common::Point3D(x, y, z) {
        }

        bool operator==(const Vertex& another) const {
            return Util::areFloatsEqual(this->x, another.x) &&
                   Util::areFloatsEqual(this->y, another.y) &&
                   Util::areFloatsEqual(this->z, another.z);    
        }

        void addFace(const unsigned faceId) {
            m_faces.push_back(faceId);
        }

        void getFaces(std::vector<unsigned>& faces) const {
            faces = m_faces;
        }

        common::Vector3D getNormal(const Mesh&) const;

    private:
        std::vector<unsigned> m_faces;
};

}

#endif
