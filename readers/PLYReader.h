#pragma once

#include "Reader.h"

namespace mv::readers {

class PLYReader : public Reader {
    public:
        MeshPointer getOutput(MeshPointer = nullptr) override;
        bool isInputFileBinary() const { return isBinary; }
        bool isInputFileLittleEndian() const { return isLittleEndian; }
        unsigned getNumberOfVertices() const { return numVertices; }
        unsigned getNumberOfFaces() const { return numFaces; }
    private:
        explicit PLYReader(std::string fileName, IMeshFactory const&);
        MeshPointer getOutput(std::istream& inputStream, MeshPointer&);
        MeshPointer readBinary(std::istream& inputStream, MeshPointer&) const;
        MeshPointer readASCII(std::istream&inputStream, MeshPointer&) const;
        void readHeader(std::istream& ifs);
        void createMesh(MeshPointer&) const;

    private:
        bool isBinary;
        bool isLittleEndian;
        unsigned numVertices;
        unsigned numFaces;

    // For testing
    friend class PLYReaderFixture;

    // Constructor is private. Grant access to the factory
    friend class ReaderFactory;
};

}
