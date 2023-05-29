#ifndef STL_READER_H
#define STL_READER_H
#include <string>
#include <utility>
#include "Mesh.h"
#include "Reader.h"

namespace mv::readers {

class STLReader : public MeshViewerObject, Reader {
    public:
        MeshPointer getOutput(bool clean) override;
    private:
        explicit STLReader(std::string  fn)
            : m_fileName(std::move(fn)) {}
        MeshPointer readBinary(std::ifstream& ifs, bool clean);
    private:
        std::string m_fileName;

    friend class ReaderFactory;
};

}

#endif
