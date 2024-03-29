#include "STLReader.h"    
#include <fstream>
using namespace std;
using namespace mv;

namespace mv::readers {

MeshPointer STLReader::getOutput() {
    // TODO: Turn on clean by default
    bool clean = false;
    ifstream ifs(fileName, ios::binary);
    if (!ifs) {
        throw std::runtime_error("Unable to open file " + fileName + '!');
    }
    string header;
    header.resize(80);
    ifs.read(header.data(), 80);
    if (!ifs) {
        throw std::runtime_error("Unable to read file" + fileName + '!');
    }
    if(header.find("solid") == string::npos) {
        return readBinary(ifs, clean);
    } else {
        throw std::runtime_error("ASCII STLs not supported!");
    }
}

MeshPointer STLReader::readBinary(ifstream& ifs, bool const clean) {
    if (ifs.gcount() != 80) {
        throw std::runtime_error("File stream in unexpected state");
    }
    // Read 4 bytes following the 80 byte header (read in the caller)
    unsigned numTris;
    ifs.read(reinterpret_cast<char*>(&numTris), 4);

    // Initialize Mesh
    MeshPointer mesh(new Mesh());
    mesh->initialize(numTris*3, numTris);

    // Read triangles
    /* foreach triangle                      - 50 bytes:
    REAL32[3] – Normal vector             - 12 bytes
    REAL32[3] – Vertex 1                  - 12 bytes
    REAL32[3] – Vertex 2                  - 12 bytes
    REAL32[3] – Vertex 3                  - 12 bytes
    UINT16    – Attribute byte count      -  2 bytes
    end  */

    unsigned vId = 0;
    char buffer[50];
    for (unsigned i = 0; i < numTris; ++i) {
        ifs.read(buffer, 50);
        for (char j = 0; j < 3; ++j) {
            float* vertex = reinterpret_cast<float*>(buffer + 12 * (j+1));
            mesh->addVertex(vertex[0], vertex[1], vertex[2]);
        }
        mesh->addFace({vId, vId + 1, vId + 2});
        vId += 3;
    }
    ifs.close();

    if (clean)
        mesh->removeDuplicateVertices();

    return mesh;
}

}
