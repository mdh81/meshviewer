#include "PLYReader.h"
#include <fstream>
#include <array>
#include "Mesh.h"

namespace {
    // Get a 3-tuple of the specified type from a space delimited string
    template<typename T, unsigned N>
    std::vector<T> getAsNTuple(std::string const& str) {
        std::vector<T> result(N);
        auto start = str.c_str();
        char* end;
        for(unsigned i = 0; i < N; ++i, start = end) {
            result[i] =
                    static_cast<T>(std::is_integral<T>::value ?
                    strtoul(start, &end, 10) : strtod(start, &end));
            if (((*end < '0' || *end > '9') && !isspace(*end) && *end != '\0') || errno == ERANGE) {
                throw std::runtime_error("Cannot parse " + str);
            }
        }
        return result;
    }
}

mv::readers::MeshPointer mv::readers::PLYReader::getOutput() {
    std::ifstream ifs(fileName, std::ios::binary);
    if (!ifs) {
        throw std::runtime_error("Unable to open file " + fileName + '!');
    }
    return getOutput(ifs);
}

mv::readers::MeshPointer mv::readers::PLYReader::getOutput(std::istream& inputStream) {
    readHeader(inputStream);
    return isBinary ? readBinary(inputStream) : readASCII(inputStream);
}

mv::readers::MeshPointer mv::readers::PLYReader::readBinary(std::istream& inputStream) {
    std::cout << "Parsing PLY data in binary format" << std::endl;
    return {};
}

mv::readers::MeshPointer mv::readers::PLYReader::readASCII(std::istream& inputStream) {
    std::cout << "Parsing PLY data in ASCII format" << std::endl;

    MeshPointer mesh = std::make_unique<Mesh>();
    mesh->initialize(numVertices, numFaces);

    std::string line;
    for (unsigned i = 0; i < numVertices; ++i) {
        getline(inputStream, line);
        auto vertex = getAsNTuple<float, 3>(line);
        mesh->addVertex(vertex.at(0), vertex.at(1), vertex.at(2));
    }

    for (unsigned i = 0; i < numFaces; ++i) {
        getline(inputStream, line);
        auto tri = getAsNTuple<unsigned, 4>(line);
        if (tri.at(0) != 3) {
            throw std::runtime_error("Only triangle meshes are supported at this time");
        }
        mesh->addFace({tri.at(1), tri.at(2), tri.at(3)});
    }
    return mesh;
}

void mv::readers::PLYReader::readHeader(std::istream& inputStream) {

    bool endHeader = false;
    bool isPly = false;
    bool hasVertices = false;
    bool hasFaces = false;
    std::string const vertexElementId = "element vertex ";
    std::string const faceElementId = "element face ";

    auto isNumber = [](std::string const& str) {
        return std::all_of(str.cbegin(), str.cend(), [](auto c) { return c >= '0' && c <= '9'; });
    };

    std::string headerLine;
    while(!endHeader) {
        getline(inputStream, headerLine);
        if (!isPly && headerLine == "ply") {
            isPly = true;
            continue;
        }
        if (isPly && !hasVertices) {
            if (headerLine.starts_with(vertexElementId)) {
                auto numVerticesStr = headerLine.substr(vertexElementId.size());
                if (!isNumber(numVerticesStr)) {
                    throw std::runtime_error("Number of vertices is invalid. Found " + numVerticesStr + " when parsing vertex element");
                }
                numVertices = std::atoi(numVerticesStr.c_str()); // NOLINT: string validated by lambda isNumber
                hasVertices = true;
                continue;
            }
        }
        if (isPly && !hasFaces) {
            if (headerLine.starts_with(faceElementId)) {
                auto numFacesStr = headerLine.substr(faceElementId.size());
                if (!isNumber(numFacesStr)) {
                    throw std::runtime_error("Number of faces is invalid. Found " + numFacesStr + " when parsing face element");
                }
                numFaces = std::atoi(numFacesStr.c_str()); // NOLINT: string validated by lambda isNumber
                hasFaces = true;
            }
            continue;
        }
        endHeader = headerLine == "end_header";
    }
    if (!isPly || !hasVertices || !hasFaces) {
        throw std::runtime_error("Invalid PLY file. Header is incorrect!");
    }

    std::cout << "Parsing PLY file with " << numVertices << " vertices and " << numFaces << " faces" << std::endl;

}
