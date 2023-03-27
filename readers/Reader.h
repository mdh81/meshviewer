#pragma once
#include <memory>

namespace mv {
    class Mesh;
}

namespace mv::readers {
using MeshPointer = std::unique_ptr<mv::Mesh>;

class Reader {
public:
    virtual MeshPointer getOutput(bool const clean = false) = 0;
    virtual ~Reader() = default;
};

}
