#pragma once
#include <string>
#include <utility>
#include "Mesh.h"
#include "Reader.h"

namespace mv::readers {

class STLReader : public MeshViewerObject, Reader {
    public:
        MeshPointer getOutput() override;
    private:
        explicit STLReader(std::string fn) : Reader(std::move(fn)) { }
        MeshPointer readBinary(std::ifstream& ifs, bool clean);

    friend class ReaderFactory;
};

}
