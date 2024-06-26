#include "MeshImpl.h"
#include "ConfigurationReader.h"
#include <limits>
#include <iostream>
#include <algorithm>
#include <unordered_set>

using namespace std;

namespace mv {

using namespace common;

MeshImpl::MeshImpl()
    : Drawable3D("MeshVertex.glsl", "Fragment.glsl", Effect::Fog)
    , m_numVertices(0)
    , m_numFaces(0)
    , m_connectivityDataSize(0) {

}

void MeshImpl::initialize(const unsigned numVertices, const unsigned numFaces) {
    m_numVertices = numVertices;
    m_numFaces = numFaces;
    m_vertices.reserve(m_numVertices);
    m_faces.reserve(m_numFaces);
}

MeshImpl::MeshImpl(MeshImpl const& another) :
        Drawable3D(another.vertexShaderFileName, another.fragmentShaderFileName, Effect::Fog) {
    m_numVertices = another.m_numVertices;
    m_numFaces = another.m_numFaces;
    m_vertices.resize(m_numVertices);
    m_connectivityDataSize = another.m_connectivityDataSize;
    std::copy(another.m_vertices.cbegin(), another.m_vertices.cend(), m_vertices.begin());
    m_faces.resize(m_numFaces);
    std::copy(another.m_faces.cbegin(), another.m_faces.cend(), m_faces.begin());
}

void MeshImpl::addVertex(const float x, const float y, const float z) {
    m_vertices.emplace_back(x, y, z);
}

void MeshImpl::addFace(const initializer_list<unsigned>& vertexIds) {
    m_faces.emplace_back(vertexIds);
    for (auto vertexId : vertexIds) {
        m_vertices.at(vertexId).addFace(m_faces.size()-1);
    }
}

const Vertex& MeshImpl::getVertex(const unsigned vertexIndex) const {
    if (vertexIndex >= m_numVertices)
        throw std::runtime_error("Vertex index invalid");
    return m_vertices.at(vertexIndex);
}

const Face& MeshImpl::getFace(unsigned faceIndex) const {
    if (faceIndex >= m_numFaces)
        throw std::runtime_error("Face index invalid");
    return m_faces.at(faceIndex);
}

Bounds MeshImpl::getBounds() const {
    if (!m_bounds) {
        const_cast<MeshImpl*>(this)->buildBounds();
    }
    return m_bounds.value();
}

void MeshImpl::buildBounds() {
    m_bounds.emplace();
    for (auto& v : m_vertices) {
        if (v.x < m_bounds->x.min) m_bounds->x.min = v.x;
        if (v.y < m_bounds->y.min) m_bounds->y.min = v.y;
        if (v.z < m_bounds->z.min) m_bounds->z.min = v.z;

        if (v.x > m_bounds->x.max) m_bounds->x.max = v.x;
        if (v.y > m_bounds->y.max) m_bounds->y.max = v.y;
        if (v.z > m_bounds->z.max) m_bounds->z.max = v.z;
    }
}

unsigned MeshImpl::removeDuplicateVertices() {
    // Build octree for faster search
    if (!m_octree) {
        m_octree.emplace(*this);
    }

    // Compare each vertex against its neighbours and see
    // if there is a duplicate in the neighborhood
    unordered_set<VertexIndex> duplicateVertices;
    unordered_map<VertexIndex, VertexIndex> remapEntries;
    size_t numDuplicates = 0;
    for (VertexIndex i = 0; i < m_vertices.size(); ++i) {
        VertexIndices neighbors;
        m_octree->getNeighboringVertices(i, neighbors);
        for (size_t ni = 0; ni < neighbors.size(); ++ni) {
            if (duplicateVertices.find(neighbors.at(ni)) == duplicateVertices.end() &&
                neighbors.at(ni) != i &&
                getVertex(neighbors.at(ni)) == getVertex(i)) {
                cout << "Vertex at " << i << ' ' << getVertex(i)
                               << " has a duplicate vertex at index " << neighbors.at(ni) << endl;
                duplicateVertices.insert(i);
                duplicateVertices.insert(neighbors.at(ni));
                remapEntries.emplace(neighbors.at(ni), i);
                ++numDuplicates;
            }
        }
    }

    // Find new indices for vertices to account for duplication
    for (auto& remapEntry : remapEntries) {
        // If vertex N were to removed as a duplicate, all vertices in the range
        // [N+1, Number of vertices-1] should be shifted to the left
        for (VertexIndex i = remapEntry.first; i < m_numVertices; ++i) {
            if (remapEntries.find(i) == remapEntries.end())
                remapEntries.emplace(i, remapEntries.find(i-1) == remapEntries.end() ? i-1 : remapEntries[i-1]-1);
        }
        if (remapEntries.find(remapEntry.second) != remapEntries.end()) {
            remapEntry.second = remapEntries[remapEntry.second];
        }
        cout << "Vertex " << remapEntry.first << ":" << remapEntry.second << endl;
    }

    // Execute remapping
    for (auto& remapEntry : remapEntries) {
        // TODO: Complete implementation of remap
    }
    if (debug)
        cout << "Removed " << numDuplicates << " duplicate vertices" << endl;
    return duplicateVertices.size();
}

Point3D MeshImpl::getCentroid() const {
    if (!m_bounds) {
        const_cast<MeshImpl*>(this)->buildBounds();
    }
    return { (this->m_bounds->x.max + this->m_bounds->x.min) * 0.5f,
             (this->m_bounds->y.max + this->m_bounds->y.min) * 0.5f,
             (this->m_bounds->z.max + this->m_bounds->z.min) * 0.5f };
}

void MeshImpl::buildVertexData() {
    m_vertexData = VertexData(m_numVertices);
    for (auto& v : m_vertices) {
        m_vertexData->append(v.x, v.y, v.z);
    }
}

MeshImpl::VertexData MeshImpl::getVertexData() const {
    if (!m_vertexData) const_cast<MeshImpl*>(this)->buildVertexData();
    return m_vertexData.value();
}

void MeshImpl::buildConnectivityData() {
    m_connectivityDataSize = 0;
    for (size_t i = 0; i < m_numFaces; ++i) {
        m_connectivityDataSize += (m_faces.at(i).size() * sizeof(unsigned));
    }
    m_connectivity.reset(new unsigned[m_connectivityDataSize]);
    for (size_t i = 0, offset = 0; i < m_numFaces; ++i) {
        size_t faceDataSize = m_faces.at(i).size() * sizeof(unsigned);
        memcpy(m_connectivity.get() + offset, m_faces.at(i).data(), faceDataSize);
        offset += m_faces.at(i).size();
    }
}

void MeshImpl::getConnectivityData(size_t& numBytes, unsigned*& pConnData) const {
    // C++ doesn't provide a way out of const cast to support lazy loading that's
    // necessary here. Not all meshes will be rendered and until a mesh is rendered
    // there is no need to get connectivity data out of a mesh like below
    if (!m_connectivity) const_cast<MeshImpl*>(this)->buildConnectivityData();
    numBytes = m_connectivityDataSize;
    pConnData = m_connectivity.get();
}

std::unique_ptr<Mesh> MeshImpl::transform(common::TransformMatrix const& transformMatrix) const {
    std::unique_ptr<Mesh> transformedMesh(new MeshImpl(*this));
    auto& vertices = transformedMesh->getVertices();
    for (auto& vertex : vertices) {
        auto& v = const_cast<Vertex&>(vertex);
        auto transformedVertex = transformMatrix * math3d::Vector4<float>{vertex.x, vertex.y, vertex.z, 1.0f};
        v.x = transformedVertex.x;
        v.y = transformedVertex.y;
        v.z = transformedVertex.z;
    }
    return transformedMesh;
}

void MeshImpl::writeToSTL(std::string const& fileName) const {

    ofstream ofs(fileName, ios::binary);
    char header[80] = "STL file generated by MeshViewer (https://github.com/mdh81/meshviewer)";
    ofs.write(header, 80);
    if (m_faces.size() > numeric_limits<unsigned>::max())
        throw std::runtime_error("Cannot write to STL. Number of faces in the mesh exceed the maximum tris supported by STL");
    auto numTris = static_cast<unsigned>(m_faces.size());
    ofs.write(reinterpret_cast<char*>(&numTris), 4);
    unsigned short dummy = 0;
    for (auto& face : m_faces) {
        if (face.size() != 3) throw std::runtime_error("Cannot write to STL. Mesh is not triangulated");
        auto& A = m_vertices.at(face.at(0));
        auto& B = m_vertices.at(face.at(1));
        auto& C = m_vertices.at(face.at(2));
        auto AB = B-A;
        auto AC = C-A;
        auto normal = (AC * AB).normalize();
        ofs.write(reinterpret_cast<const char*>(normal.getData()), 12);
        ofs.write(reinterpret_cast<const char*>(A.getData()), 12);
        ofs.write(reinterpret_cast<const char*>(B.getData()), 12);
        ofs.write(reinterpret_cast<const char*>(C.getData()), 12);
        ofs.write(reinterpret_cast<char*>(&dummy), 2);
    }
    ofs.close();
}

void MeshImpl::generateVertexNormals() {
    if (m_vertexNormals) return;
    m_vertexNormals = NormalData(m_numVertices);
    for (size_t i = 0; i < m_numVertices; ++i) {
        auto normal = m_vertices.at(i).getNormal(*this);
        m_vertexNormals->append(normal[0], normal[1], normal[2]);
    }
}

void MeshImpl::generateFaceNormals() {
    if (m_faceNormals) return;
    m_faceNormals = NormalData(m_numFaces);
    for (size_t i = 0; i < m_numFaces; ++i) {
        auto normal = m_faces.at(i).getNormal(*this);
        m_faceNormals->append(normal[0], normal[1], normal[2]);
    }
}

MeshImpl::NormalData MeshImpl::getNormals(const NormalLocation location) const {
    if (location == NormalLocation::Vertex) {
        if (!m_vertexNormals)
            const_cast<MeshImpl*>(this)->generateVertexNormals();
        return m_vertexNormals.value();
    } else {
        if (!m_faceNormals)
            const_cast<MeshImpl*>(this)->generateFaceNormals();
        return m_faceNormals.value();
    }
}

void MeshImpl::generateRenderData() {

    if (readyToRender) return;

    // Compile and link shaders
    createShaderProgram();

    // Switch to mesh shader program
    glUseProgram(shaderProgram);

    // Create mesh vertex array object
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    // Create mesh vertex buffer object
    GLuint meshVbo;
    glGenBuffers(1, &meshVbo);
    glBindBuffer(GL_ARRAY_BUFFER, meshVbo);

    // Push vertex data to graphics card
    const auto vertexData = getVertexData();
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexData.getDataSize()), vertexData.getData(),
                 GL_STATIC_DRAW);

    // Define layout of vertex data
    GLint posAttrib = glGetAttribLocation(shaderProgram, "vertexModel");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib,            //attrib identifier
                          3,                    //number of values for this attribute
                          GL_FLOAT,             //data type
                          GL_FALSE,             //data normalization status
                          3 * sizeof(float),      //stride--each vertex has 3 float entries
                          nullptr               //offset into the array
    );

    // Create VBO for normals
    GLuint meshNormalsVbo;
    glGenBuffers(1, &meshNormalsVbo);

    // Make the normals vertex buffer object the current buffer
    glBindBuffer(GL_ARRAY_BUFFER, meshNormalsVbo);

    // Upload normals to the vertex buffer object
    auto const normalData = getNormals(common::NormalLocation::Vertex);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(normalData.getDataSize()), normalData.getData(), GL_STATIC_DRAW);

    // Define layout of normal data
    GLint normalAttrib = glGetAttribLocation(shaderProgram, "vertexNormalModel");
    glEnableVertexAttribArray(normalAttrib);
    glVertexAttribPointer(normalAttrib,         //attrib identifier
                          3,                    //number of values for this attribute
                          GL_FLOAT,             //data type
                          GL_FALSE,             //data normalization status
                          3 * sizeof(float),      //stride--each normal has 3 float entries
                          nullptr                     //offset into the array
    );

    // Define element data
    // Create element buffer object
    glGenBuffers(1, &elementBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);

    // Upload connectivity data to element buffer object
    size_t numBytes;
    GLuint *faceData;
    getConnectivityData(numBytes, faceData);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(numBytes), faceData, GL_STATIC_DRAW);

    generateColors();
    readyToRender = true;

    // TODO: Dump member data buffers
}

void MeshImpl::generateColors() {
    if (readyToRender) return;

    auto& cfgReader = config::ConfigurationReader::getInstance();

    // Set mesh colors
    GLint diffuseColorId = glCallWithErrorCheck(glGetUniformLocation, shaderProgram, "material.diffuseColor");
    glCallWithErrorCheck(glUniform3fv, diffuseColorId, 1, cfgReader.getColor("MeshDiffuseColor", true).getData());
    GLint ambientColorId = glCallWithErrorCheck(glGetUniformLocation, shaderProgram, "material.ambientColor");
    glCallWithErrorCheck(glUniform3fv, ambientColorId, 1, cfgReader.getColor("MeshAmbientColor", true).getData());
    GLint specularColorId = glCallWithErrorCheck(glGetUniformLocation, shaderProgram, "material.specularColor");
    glCallWithErrorCheck(glUniform3fv, specularColorId, 1, cfgReader.getColor("MeshSpecularColor", true).getData());
    GLint shininessId = glCallWithErrorCheck(glGetUniformLocation, shaderProgram, "material.shininess");
    glCallWithErrorCheck(glUniform1f, shininessId, std::atof(cfgReader.getValue("MeshShininess").c_str()));

    // TODO: This moves to the viewport.
    // Set light position in view coordinates
    // The default light is a simple headlight that's positioned at the
    // camera's location. Camera in GL is considered to be global origin
    GLint lightPosId = glCallWithErrorCheck(glGetUniformLocation, shaderProgram, "light.position");
    glCallWithErrorCheck(glUniform3fv, lightPosId, 1, cfgReader.getVector("LightPosition").getData());

    // Set light intensity
    GLint lightIntensityId = glCallWithErrorCheck(glGetUniformLocation, shaderProgram, "light.color");
    glCallWithErrorCheck(glUniform3fv, lightIntensityId, 1, cfgReader.getColor("LightColor", true).getData());
}

void MeshImpl::render() {

    if (!readyToRender) {
        generateRenderData();
    }

    glUseProgram(shaderProgram);

    // Send matrices to the shader
    setTransforms();

    glBindVertexArray(vertexArrayObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
#ifndef EMSCRIPTEN
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
    glDrawElements(GL_TRIANGLES,
                   static_cast<int>(m_numFaces*3),              // Number of entries in the connectivity array
                   GL_UNSIGNED_INT,                              // Type of element buffer data
                   nullptr                                     // Offset into element buffer data
                  );
}

unsigned MeshImpl::getNumberOfVertices() const  { return m_numVertices; };

unsigned MeshImpl::getNumberOfFaces() const { return m_numFaces; }

// Gets all vertices in this mesh
Mesh::Vertices const& MeshImpl::getVertices() const { return m_vertices; }

// Gets all faces in this mesh
Mesh::Faces const& MeshImpl::getConnectivity() const { return m_faces; }

void MeshImpl::writeToFile(std::string const& fileName, common::TransformMatrix const& transform) const {
    std::filesystem::path filePath = fileName;
    if (filePath.extension() == ".STL" || filePath.extension() == ".stl") {
        this->transform(transform)->writeToSTL(fileName);
    } else {
        throw std::runtime_error("Currently, only STL output for meshes is supported.");
    }
}

}
