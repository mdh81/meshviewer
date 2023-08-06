#pragma once

#include "MeshViewerObject.h"
#include "Reader.h"

namespace mv::readers {

class PLYReader : public MeshViewerObject, Reader {
    public:
        MeshPointer getOutput() override;
        bool isInputFileBinary() const { return isBinary; }
        bool isInputFileLittleEndian() const { return isLittleEndian; }
        unsigned getNumberOfVertices() const { return numVertices; }
        unsigned getNumberOfFaces() const { return numFaces; }
    private:
        explicit PLYReader(std::string fn)
        : Reader(std::move(fn))
        , isBinary(false)
        , isLittleEndian(false)
        , numVertices(0)
        , numFaces(0) { }
        MeshPointer getOutput(std::istream& inputStream);
        MeshPointer readBinary(std::istream& inputStream);
        MeshPointer readASCII(std::istream&inputStream);
        void readHeader(std::istream& ifs);

    private:
        bool isBinary;
        bool isLittleEndian;
        unsigned numVertices;
        unsigned numFaces;

        // For testing
        static PLYReader createForTest() {
            return PLYReader("");
        }
        friend class PLYReaderFixture;

    friend class ReaderFactory;
};

}
