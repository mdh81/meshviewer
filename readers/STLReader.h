#ifndef STL_READER_H
#define STL_READER_H
#include <string>
#include "Mesh.h"
#include "Reader.h"

namespace mv::readers {

class STLReader : public MeshViewerObject, Reader {
    public:
        MeshPointer getOutput(bool const clean) override;
    private:
        STLReader(const std::string& fn)
            : m_fileName(fn) {}
        MeshPointer readBinary(std::ifstream& ifs, bool const clean);
    private:
        std::string m_fileName;

    friend class ReaderFactory;
};

}

#endif
