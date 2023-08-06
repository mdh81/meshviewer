#pragma once
#include <memory>
#include <utility>

namespace mv {
    class Mesh;
}

namespace mv::readers {
using MeshPointer = std::unique_ptr<mv::Mesh>;

class Reader {
public:
    explicit Reader(std::string fileName) : fileName(std::move(fileName)) {}
    virtual MeshPointer getOutput() = 0;
    virtual ~Reader() = default;

protected:
    std::string const fileName;
};

}
