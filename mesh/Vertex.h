#ifndef MESH_VIEWER_VERTEX_H
#define MESH_VIEWER_VERTEX_H

#include "Types.h"
#include "3dmath/Vector.h"
#include <vector>

namespace meshviewer { 

class Mesh;

class Vertex : public common::Point3D {

    public:
        // For allowing instances of this type to be
        // stored in containers like vector
        Vertex() = default;

        Vertex(float x, float y, float z) { 
            this->x = x;
            this->y = y;
            this->z = z;
        }

        math3d::Vector<float, 3> operator-(const Vertex& another) const {
            return math3d::Vector<float,3>({x - another.x, y - another.y, z - another.z});
        }

        void addFace(const unsigned faceId) {
            std::cout << "Adding face " << faceId << std::endl;
            m_faces.push_back(faceId);
        }

        void getFaces(std::vector<unsigned>& faces) const {
            faces = m_faces;
        }

        math3d::Vector<float, 3> getNormal(const Mesh&) const; 

    private:
        std::vector<unsigned> m_faces;
};

}

#endif
