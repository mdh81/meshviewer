#include "GradientBackground.h"
#include "ConfigurationReader.h"
#include "CallbackFactory.h"
#include "EventHandler.h"
#include <vector>
#include <limits>
using namespace std;
using namespace mv::common;
using namespace mv::events;
using namespace mv::config;

namespace mv::objects {

using byte = mv::common::byte;

GradientBackground::GradientBackground(const GradientType type, const GradientDirection direction)
: Renderable("BackgroundVertex.glsl", "Fragment.glsl")
, m_direction(direction)
, m_type(type) {
    EventHandler().registerCallback(
            Event(GLFW_KEY_H),
            CallbackFactory::getInstance().registerCallback
                    (*this, &GradientBackground::setGradientDirection, GradientDirection::Horizontal));

    EventHandler().registerCallback(
            Event(GLFW_KEY_V),
            CallbackFactory::getInstance().registerCallback
                    (*this, &GradientBackground::setGradientDirection, GradientDirection::Vertical));
}

void GradientBackground::render(const mv::Camera &camera) {

    if (!m_readyToRender) {
        generateRenderData();
    }

    glCallWithErrorCheck(glUseProgram, m_shaderProgram);
    glCallWithErrorCheck(glBindVertexArray, m_vertexArrayObject);
    glCallWithErrorCheck(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, m_elementBufferObject);
    glCallWithErrorCheck(glDrawElements, GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
}

void GradientBackground::generateRenderData() {

    if (m_readyToRender) {
        return;
    }

    constexpr byte numberOfStops = 3;

    // Create shader program
    createShaderProgram();
    glCallWithErrorCheck(glUseProgram, m_shaderProgram);

    // Create vertex data
    glCallWithErrorCheck(glGenVertexArrays, 1, &m_vertexArrayObject);
    glCallWithErrorCheck(glBindVertexArray, m_vertexArrayObject);
    GLuint gradientVbo;
    glCallWithErrorCheck(glGenBuffers, 1, &gradientVbo);
    glCallWithErrorCheck(glBindBuffer, GL_ARRAY_BUFFER, gradientVbo);

    // The linear gradient pattern used is color1->color2->color1
    vector<Color> gradientColors(numberOfStops);
    gradientColors[0] = ConfigurationReader::getInstance().getColor("GradientColor1");
    gradientColors[1] = ConfigurationReader::getInstance().getColor("GradientColor2");
    gradientColors[2] = ConfigurationReader::getInstance().getColor("GradientColor1");

    // Vertices are defined in normalized device coordinates, which is a left-handed system.
    // Therefore, the right location for background is the XY plane z = 1
    // The vertex and color data are interleaved in the buffer whose stride is 6 with
    // the vertex coordinates taking the first three values and the color components taking
    // the next three
    auto bufferSize = 6 * numberOfStops * 2;
    vector<GLfloat> vertexData(bufferSize);
    byte dataIndex = 0;
    // NDC runs from -1 to +1 in all directions
    GLfloat x = -1.f, y = -1.f;
    // NDC is a cube of length 2. We will divide the vertical or horizontal length
    // by the number of gradient stops
    GLfloat increment = 2.f / (numberOfStops-1);
    for (byte i = 0; i < 2; ++i) { // Loop over end points
        for (byte j = 0; j < numberOfStops; ++j) { // Loop over gradient stops
            // vertex coordinates
            vertexData[dataIndex++] = x;
            vertexData[dataIndex++] = y;
            vertexData[dataIndex++] = 1 - 1e-6;
            // color components
            vertexData[dataIndex++] = gradientColors[j].x;
            vertexData[dataIndex++] = gradientColors[j].y;
            vertexData[dataIndex++] = gradientColors[j].z;
            if (m_direction == GradientDirection::Vertical) {
                y += increment;
            } else {
                x += increment;
            }
        }
        if (m_direction == GradientDirection::Vertical) {
            x = +1.f;
            y = -1.f;
        } else {
            x = -1.f;
            y = +1.f;
        }
    }

    // Upload data to the gpu
    glCallWithErrorCheck(glBufferData, GL_ARRAY_BUFFER, sizeof(GLfloat) * bufferSize, vertexData.data(), GL_STATIC_DRAW);

    // Specify layout of vertex data
    // Coordinates
    auto positionAttrib = glCallWithErrorCheck(glGetAttribLocation, m_shaderProgram, "vertexPosition");
    glCallWithErrorCheck(glEnableVertexAttribArray, positionAttrib);
    glCallWithErrorCheck(glVertexAttribPointer, positionAttrib,
                         3,                 // Entries per vertex
                         GL_FLOAT,          // Format
                         GL_FALSE,          // Normalized
                         6*sizeof(GLfloat), // Stride
                         nullptr);          // Offset
    // Color
    auto colorAttrib = glCallWithErrorCheck(glGetAttribLocation, m_shaderProgram, "vertexColorIn");
    glCallWithErrorCheck(glEnableVertexAttribArray, colorAttrib);
    glCallWithErrorCheck(glVertexAttribPointer, colorAttrib,
                         3,                             // Entries per vertex
                         GL_FLOAT,                      // Format
                         GL_FALSE,                      // Normalized
                         6*sizeof(GLfloat),             // Stride
                         (void*)(3*sizeof(GLfloat)));   // Offset

    // Specify connectivity
    glCallWithErrorCheck(glGenBuffers, 1, &m_elementBufferObject);
    glCallWithErrorCheck(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, m_elementBufferObject);
    // m_numberOfStops - 1 is the number of quads we will draw on the far plane
    // Each quad will be split into two triangles, each of which will have three indices
    vector<GLuint> faces((numberOfStops - 1) * 2 * 3);
    faces[0] = 0; faces[1] = 4;  faces[2] = 3;
    faces[3] = 0; faces[4] = 4;  faces[5] = 1;
    faces[6] = 1; faces[7] = 4;  faces[8] = 5;
    faces[9] = 1; faces[10] = 5; faces[11] = 2;
    glCallWithErrorCheck(glBufferData, GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(GLuint), faces.data(), GL_STATIC_DRAW);

    m_readyToRender = true;
}

}
