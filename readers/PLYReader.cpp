#include "PLYReader.h"
#include <algorithm>
#include "MeshFactory.h"

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

    using MeshPointer = mv::readers::Reader::MeshPointer;
}

mv::readers::PLYReader::PLYReader(std::string fileName, IMeshFactory const &meshFactory)
    : Reader(std::move(fileName), meshFactory)
    , isBinary(false)
    , isLittleEndian(false)
    , numVertices(0)
    , numFaces(0) {
}

MeshPointer mv::readers::PLYReader::getOutput(MeshPointer mesh) {
    std::ifstream ifs(fileName, std::ios::binary);
    if (!ifs) {
        throw std::runtime_error("Unable to open file " + fileName + '!');
    }
    return getOutput(ifs, mesh);
}

MeshPointer mv::readers::PLYReader::getOutput(std::istream& inputStream, MeshPointer& mesh) {
    readHeader(inputStream);
    return isBinary ? readBinary(inputStream, mesh) : readASCII(inputStream, mesh);
}

void mv::readers::PLYReader::createMesh(MeshPointer& mesh) const {
    // This is to allow a mock mesh to be injected into this object for unit testing
    if (!mesh) {
        mesh = meshFactory.createMesh();
    }
    mesh->initialize(numVertices, numFaces);
}

MeshPointer mv::readers::PLYReader::readBinary(std::istream& ifs, MeshPointer& mesh) const {
    std::cout << "Parsing PLY data in binary format" << std::endl;
    createMesh(mesh);
    // Assumptions:
    // 1. Vertex data type is float
    // 2. Mesh is a triangle mesh
    // 3. Num Vertices entry is a byte
    // 5. Vertex indices are 4-bytes
    {
        auto const vertexDataNumBytes = numVertices * sizeof(float) * 3;
        std::unique_ptr<char[]> const vertexData{new char[vertexDataNumBytes]};
        ifs.read(vertexData.get(), vertexDataNumBytes);
        for (unsigned i = 0; i < numVertices; ++i) {
            auto const xyz = reinterpret_cast<float*>(vertexData.get() + 3 * sizeof(float) * i);
            mesh->addVertex(xyz[0], xyz[1], xyz[2]);
        }
    }
    auto const currentPosition = ifs.tellg();
    ifs.seekg(0, std::ios_base::end);
    auto const size = ifs.tellg();
    auto constexpr tupleSize = 3 * sizeof(unsigned int) + sizeof(char);
    auto const indexDataNumBytes = numFaces * tupleSize;
    if (size - currentPosition != indexDataNumBytes) {
        throw std::runtime_error{"Data is invalid. It's possible mesh has mixed element types"
                                 " Only triangle meshes are supported at this time"};
    }

    {
        ifs.seekg(currentPosition, std::ios_base::beg);
        std::unique_ptr<char[]> const indexData{new char[indexDataNumBytes]};
        ifs.read(indexData.get(), indexDataNumBytes);
        for (unsigned i = 0; i < numFaces; ++i) {
            auto const tuple = indexData.get() + i * tupleSize;
            auto const indices = reinterpret_cast<unsigned int*>(&tuple[1]);
            mesh->addFace({indices[0], indices[1], indices[2]});
        }
    }
    return std::move(mesh);
}

MeshPointer mv::readers::PLYReader::readASCII(std::istream& inputStream, MeshPointer& mesh) const {
    std::cout << "Parsing PLY data in ASCII format" << std::endl;
    createMesh(mesh);
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
    return std::move(mesh);
}

void mv::readers::PLYReader::readHeader(std::istream& inputStream) {

    bool endHeader = false;
    bool isPly = false;
    bool hasVertices = false;
    bool hasFaces = false;
    bool hasFormat = false;
    std::string const vertexElementId = "element vertex ";
    std::string const faceElementId = "element face ";
    std::string const formatId = "format ";

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
        if (isPly && !hasFormat) {
            if (headerLine.starts_with(formatId)) {
                auto format = headerLine.substr(formatId.size());
                isBinary = format.starts_with("binary_little_endian");
                hasFormat = true;
            }
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
    if (!isPly || !hasVertices || !hasFaces || !hasFormat) {
        throw std::runtime_error("Invalid PLY file. Header is incorrect!");
    }

    std::cout << "Parsing PLY file with " << numVertices << " vertices and " << numFaces << " faces" << std::endl;

}
