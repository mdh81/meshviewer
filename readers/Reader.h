#pragma once
#include <memory>
#include "MeshViewerObject.h"
#include "MeshFactory.h"
#include <utility>

namespace mv::readers {

class Reader : public MeshViewerObject {
public:
    using MeshPointer = Mesh::MeshPointer;
    explicit Reader(std::string fileName, IMeshFactory const& meshFactory)
    : fileName(std::move(fileName))
    , meshFactory(meshFactory) {}
    virtual Mesh::MeshPointer getOutput(Mesh::MeshPointer = nullptr) = 0;
    virtual ~Reader() = default;

protected:
    std::string const fileName;
    IMeshFactory const& meshFactory;
};

}
