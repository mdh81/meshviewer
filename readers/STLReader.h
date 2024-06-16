#pragma once
#include <string>
#include <utility>
#include "Reader.h"

namespace mv::readers {

class STLReader : public Reader {
    public:
        MeshPointer getOutput(Mesh::MeshPointer = nullptr) override;
    private:
        MeshPointer getOutput(std::ifstream&, Mesh::MeshPointer&, bool clean = false);
        explicit STLReader(std::string fileName, IMeshFactory const&);
        MeshPointer readBinary(std::ifstream&, bool clean, Mesh::MeshPointer&);

    friend class STLReaderFixture;
    friend class ReaderFactory;
};

}
