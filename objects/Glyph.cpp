#include "Glyph.h"
#include "Mesh.h"
#include "glm/gtc/type_ptr.hpp"
using namespace std;

namespace mv {

using namespace common;

    [[maybe_unused]] Glyph::Glyph(Mesh const& mesh, const GlyphAssociation assoc)
    : Renderable("GlyphVertex.shader", "Fragment.shader")
    , m_mesh(mesh)
    , m_association(assoc)
    , m_numGlyphs(0) {
}

void Glyph::generateRenderData() {

    if (m_association == GlyphAssociation::Undefined)
        throw std::runtime_error("Mesh association is undefined for glyph");

    // compile and link shaders
    createShaderProgram();

    // Switch to glyph shader program
    glUseProgram(m_shaderProgram);

    // Set shader inputs
    GLint colorId = glGetUniformLocation(m_shaderProgram, "lineColor");
    glUniform3fv(colorId, 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 0.0f)));

    // Create VBO
    GLuint vbObj;
    glGenBuffers(1, &vbObj);

    // Make the glyph vertex buffer object the current buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbObj);

    if (m_association != GlyphAssociation::VertexNormal &&
        m_association != GlyphAssociation::FaceNormal)
        throw std::runtime_error("Mesh association for glyph is not supported");

    size_t dataSize = 0;
    if (m_association == GlyphAssociation::VertexNormal)
        dataSize = buildVertexNormalData();
    else
        dataSize = buildFaceNormalData();

    // Push data to the buffer
    glBufferData(GL_ARRAY_BUFFER, dataSize, m_vertexData.get(), GL_STATIC_DRAW);

    // Create vertex array object for glyph
    glGenVertexArrays(1, &m_vertexArrayObject);
    glBindVertexArray(m_vertexArrayObject);

    // Define layout of glyph data
    GLint posAttrib = glGetAttribLocation(m_shaderProgram, "vertexPosition");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib,            //attrib identifier
                          3,                    //number of values for this attribute
                          GL_FLOAT,             //data type
                          GL_FALSE,             //data normalization status
                          3*sizeof(float),      //stride--each glyph end point has 3 float entries
                          0                     //offset into the array
                         );

    // Create EBO
    GLuint ebo;
    glGenBuffers(1, &m_elementBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBufferObject);

    // Upload connectivity data to element buffer object
    // Each glyph has two end points and the data type is unsigned int
    dataSize = 2 * m_numGlyphs * sizeof(unsigned);
    m_elementData = std::make_unique<unsigned[]>(2*m_numGlyphs);
    for (size_t i = 0; i < m_numGlyphs; ++i) {
        m_elementData[2*i] = 2*i;
        m_elementData[2*i+1] = 2*i+1;
    }

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSize, m_elementData.get(), GL_STATIC_DRAW);

    generateColors();
}

size_t Glyph::buildVertexNormalData() {

    auto const normalData =
        m_mesh.getNormals(common::NormalLocation::Vertex);

    // Render normal glyphs a 10th of the mesh size
    auto scale = m_mesh.getBounds().len() * 0.1f;

    // Create vertex data
    size_t numBytes = 0;

    m_numGlyphs = m_mesh.getNumberOfVertices();

    // 6 floats per glyph, 3 floats per end point
    m_vertexData = std::make_unique<float[]>(6*m_numGlyphs);
    numBytes = (6*m_numGlyphs) * sizeof(float);
    for (size_t i = 0; i < m_numGlyphs; ++i) {
        const auto& v = m_mesh.getVertex(i);
        const auto& vn = v.getNormal(m_mesh);
        // End point 1
        m_vertexData[6*i]   = v.x;
        m_vertexData[6*i+1] = v.y;
        m_vertexData[6*i+2] = v.z;
        // End point 2
        m_vertexData[6*i+3] = v.x + scale * (vn[0]);
        m_vertexData[6*i+4] = v.y + scale * (vn[1]);
        m_vertexData[6*i+5] = v.z + scale * (vn[2]);
    }
    return numBytes;
}

size_t Glyph::buildFaceNormalData() {

    auto const normalData =
        m_mesh.getNormals(common::NormalLocation::Face);

    // Render normal glyphs a 10th of the mesh size
    auto scale = m_mesh.getBounds().len() * 0.1f;

    // 6 floats per glyph, 3 floats per end point
    m_numGlyphs = m_mesh.getNumberOfFaces();
    m_vertexData = std::make_unique<float[]>(6*m_numGlyphs);
    size_t numBytes = (6*m_numGlyphs) * sizeof(float);
    for (size_t i = 0; i < m_numGlyphs; ++i) {
        const auto& f = m_mesh.getFace(i);
        const auto& c = m_mesh.getFace(i).getCentroid(m_mesh);
        const auto& fn = f.getNormal(m_mesh);
        // End point 1
        m_vertexData[6*i]   = c.x;
        m_vertexData[6*i+1] = c.y;
        m_vertexData[6*i+2] = c.z;
        // End point 2
        m_vertexData[6*i+3] = c.x + scale * (fn[0]);
        m_vertexData[6*i+4] = c.y + scale * (fn[1]);
        m_vertexData[6*i+5] = c.z + scale * (fn[2]);
    }
    return numBytes;
}

void Glyph::render(Camera const& camera) {

    if (!m_readyToRender) {
        const_cast<Glyph&>(*this).generateRenderData();
    }

    // Switch to glyph shader program
    glUseProgram(m_shaderProgram);

    // Set transform shader inputs
    setModelViewProjection(camera);

    // Bind vertex array object and element buffer object
    glBindVertexArray(m_vertexArrayObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBufferObject);

    // Render lines
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBufferObject);
    glDrawElements(GL_LINES,
                   m_numGlyphs,                // Number of elements
                   GL_UNSIGNED_INT,             // Type of element buffer data
                   0                          // Offset into element buffer data
                  );

}

void Glyph::generateColors() {
    if (m_readyToRender) return;
}

}
