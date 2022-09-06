#ifndef MESH_VIEWER_FACE_H
#define MESH_VIEWER_FACE_H

#include "Vertex.h"
#include "3dmath/Vector.h"
#include "initializer_list"

namespace meshviewer {

class Mesh;

class Face {
    public:
        // Defaulted constructor to allow instances to be
        // stored in containers like vector. Some operations like
        // resize expect a default constructor to be present
        Face() = default;

        Face(const std::initializer_list<unsigned>& vertexIds) {
            m_vertexIds = vertexIds;
        }

        math3d::Vector<float, 3> getNormal(const Mesh&) const;

        size_t size() const { 
            return m_vertexIds.size(); 
        }

        void getVertices(std::vector<unsigned>& vertices) const { 
            vertices = m_vertexIds; 
        }

        unsigned at(unsigned i) const { 
            if (i >= m_vertexIds.size())
               throw std::invalid_argument("Index " + std::to_string(i) + 
                                           " is out of bounds. Face has " + 
                                           std::to_string(m_vertexIds.size()) + " vertices"); 
            return std::data(m_vertexIds)[i];
        }

        unsigned operator[](unsigned i) const {
            return at(i);
        }

        const unsigned* data() const { 
            return std::data(m_vertexIds);
        } 

    private:
        std::vector<unsigned> m_vertexIds;            

    friend std::ostream& operator<<(std::ostream&, const Face&);
};

inline std::ostream& operator<<(std::ostream& os, const Face& f) {
    os << '[';
    for (size_t i = 0; i < f.m_vertexIds.size(); ++i) { 
        os << data(f.m_vertexIds)[i] << (i == f.m_vertexIds.size() - 1 ? ']' : ','); 
    }
    return os;
}

}
#endif
