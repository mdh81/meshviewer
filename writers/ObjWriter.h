#pragma once

#include <string>
#include <utility>
#include <vector>
#include "Types.h"
#include "3dmath/TypeAliases.h"

namespace mv::writers {

class ObjWriter {
public:
    ObjWriter(std::string const& fileName) : m_fileName(fileName) { } // NOLINT

    void writeLines(common::Lines const& points);
    void writeTriangles(common::Points const& points, common::Triangles const& faces);
    void writeAsTriangles(common::Points const& points, common::VertexIndices const& faces, common::byte vertexStride);

    // To test GPU buffers
    void writeTriangles(float const* vertices, size_t numVertices, unsigned const* tris, size_t numTris);

private:
    std::string m_fileName;
};

}