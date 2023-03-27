#pragma once

#include <string>
#include <utility>
#include <vector>
#include "Types.h"

namespace mv::writers {

class ObjWriter {
public:
    ObjWriter(std::string const& fileName) : m_fileName(fileName) { }

    void writeLines(common::Lines const& points);
    void writeTriangles(common::Points const& points, common::Triangles const& faces);
    void writeAsTriangles(common::Points const& points, common::VertexIndices const& faces, common::byte const vertexStride);

private:
    std::string m_fileName;
};

}