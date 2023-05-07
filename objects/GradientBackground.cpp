#include "GradientBackground.h"
#include "ConfigurationReader.h"
#include "CallbackFactory.h"
#include "EventHandler.h"
#include "3dmath/OrthographicProjectionMatrix.h"
#include <vector>
#include <cmath>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace mv::common;
using namespace mv::events;
using namespace mv::config;

namespace mv::objects {

using byte = mv::common::byte;

GradientBackground::GradientBackground(GradientType const type, GradientDirection const direction)
: Renderable("BackgroundVertex.glsl", "Fragment.glsl")
, m_direction(direction)
, m_type(type)
, m_numberOfStops (3)
, m_numberOfConnectivityEntries (0) {
    EventHandler().registerCallback(
            Event(GLFW_KEY_H),
    CallbackFactory::getInstance().registerCallback
                (*this, &GradientBackground::setGradientDirection, GradientDirection::Horizontal));

    EventHandler().registerCallback(
            Event(GLFW_KEY_V),
    CallbackFactory::getInstance().registerCallback
                (*this, &GradientBackground::setGradientDirection, GradientDirection::Vertical));

    EventHandler().registerCallback(
            Event(GLFW_KEY_G, GLFW_MOD_ALT),
    CallbackFactory::getInstance().registerCallback
                (*this, &GradientBackground::toggleGradientType));

    // TODO: Remove after aspect ratio problem is sorted out
    //m_debugOn = true;
}

void GradientBackground::render() {

    if (!readyToRender) {
        generateRenderData();
    }

    // Since we want the background to be behind everything else on the viewport, we draw the
    // background as the first rendered item in a frame, and we also make sure the background
    // doesn't write to the depth buffer. This way, every object that is drawn after the background
    // overwrites the background's fragments and "appear" on top of the background
    glCallWithErrorCheck(glDisable, GL_DEPTH_TEST);
    glCallWithErrorCheck(glUseProgram, shaderProgram);
    glCallWithErrorCheck(glBindVertexArray, vertexArrayObject);
    glCallWithErrorCheck(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
    if (m_debugOn) {
        glCallWithErrorCheck(glPolygonMode,GL_FRONT_AND_BACK, GL_LINE);
    }
    glCallWithErrorCheck(glDrawElements, GL_TRIANGLES, m_numberOfConnectivityEntries, GL_UNSIGNED_INT, nullptr);
    glCallWithErrorCheck(glEnable, GL_DEPTH_TEST);
}

void GradientBackground::generateRenderData() {

    if (readyToRender) {
        return;
    }

    // TODO: Add dirty flag so shaders won't be recreated on geometry update
    // Create shader program
    createShaderProgram();
    glCallWithErrorCheck(glUseProgram, shaderProgram);

    // TODO: Delete existing buffers
    // Create vertex data
    glCallWithErrorCheck(glGenVertexArrays, 1, &vertexArrayObject);
    glCallWithErrorCheck(glBindVertexArray, vertexArrayObject);
    GLuint gradientVbo;
    glCallWithErrorCheck(glGenBuffers, 1, &gradientVbo);
    glCallWithErrorCheck(glBindBuffer, GL_ARRAY_BUFFER, gradientVbo);

    if (m_type == GradientType::Linear) {
        generateLinearGradient();
    } else {
        generateSphericalGradient();
    }

    // Specify layout of vertex data
    // The vertex and color data are interleaved in the buffer whose stride is 6 with
    // the vertex coordinates taking the first three values and the color components taking
    // the next three
    // Coordinates
    auto positionAttrib = glCallWithErrorCheck(glGetAttribLocation, shaderProgram, "vertexCameraIn");
    glCallWithErrorCheck(glEnableVertexAttribArray, positionAttrib);
    glCallWithErrorCheck(glVertexAttribPointer, positionAttrib,
                         3,                 // Entries per vertex
                         GL_FLOAT,          // Format
                         GL_FALSE,          // Normalized
                         6*sizeof(GLfloat), // Stride
                         nullptr);          // Offset
    // Color
    auto colorAttrib = glCallWithErrorCheck(glGetAttribLocation, shaderProgram, "vertexColorIn");
    glCallWithErrorCheck(glEnableVertexAttribArray, colorAttrib);
    glCallWithErrorCheck(glVertexAttribPointer, colorAttrib,
                         3,                             // Entries per vertex
                         GL_FLOAT,                      // Format
                         GL_FALSE,                      // Normalized
                         6*sizeof(GLfloat),             // Stride
                         (void*)(3*sizeof(GLfloat)));   // Offset

    // Vertices are defined in camera or eye coordinates. They are transformed to the NDC by
    // the following orthographic projection matrix. We allow the geometry for linear gradient to be scaled
    // to fit the window without concern for aspect ratio. We fix the view volume's aspect ratio to the window's
    // in case of spherical gradient so keep the circles in the center of the geometry circles (Maybe we ought to
    // generate new mesh that adjusts for aspect ratio)
    auto matrixId = glCallWithErrorCheck(glGetUniformLocation, shaderProgram, "orthographicProjectionMatrix");
    if (m_type == GradientType::Radial) {
        Bounds geometryBounds;
        math3d::Extent<float> viewX{-aspectRatio, aspectRatio};
        geometryBounds = {{viewX.min, viewX.max},
                          {-1.f,      +1.f},
                          {-1.f,      1.f}};
        OrthographicProjectionMatrix projectionMatrix{geometryBounds};
        glCallWithErrorCheck(glUniformMatrix4fv, matrixId, 1, GL_FALSE, projectionMatrix);
    } else {
        auto identityMatrix = math3d::IdentityMatrix<float, 4, 4>{};
        glCallWithErrorCheck(glUniformMatrix4fv, matrixId, 1, GL_FALSE, identityMatrix);
    }

    readyToRender = true;
}

void GradientBackground::generateLinearGradient() {

    // The linear gradient pattern used is edge color->center color->edge color
    vector<Color> gradientColors(m_numberOfStops);
    gradientColors[0] = ConfigurationReader::getInstance().getColor("LinearGradientEdgeColor");
    gradientColors[1] = ConfigurationReader::getInstance().getColor("LinearGradientCenterColor");
    gradientColors[2] = ConfigurationReader::getInstance().getColor("LinearGradientEdgeColor");

    // Each stop has two end points, and each point has 6 values: 3 vertex and 3 color components
    auto bufferSize = m_numberOfStops * 2 * 6;
    vector<GLfloat> vertexData(bufferSize);
    byte dataIndex = 0;
    // We set -1,-1 as the left corner and +1,+1 as the right corner
    GLfloat x = -1.f, y = -1.f;
    // NDC is a cube of length 2. We will divide the vertical or horizontal length
    // by the number of gradient stops
    GLfloat increment = 2.f / static_cast<float>(m_numberOfStops-1);
    for (byte i = 0; i < 2; ++i) { // Loop over end points
        for (byte j = 0; j < m_numberOfStops; ++j) { // Loop over gradient stops
            // vertex coordinates
            vertexData[dataIndex++] = x;
            vertexData[dataIndex++] = y;
            vertexData[dataIndex++] = 0;
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
    glCallWithErrorCheck(glBufferData,
                         GL_ARRAY_BUFFER,
                         sizeof(GLfloat) * bufferSize,
                         vertexData.data(), GL_STATIC_DRAW);

    // Specify connectivity
    glCallWithErrorCheck(glGenBuffers, 1, &elementBufferObject);
    glCallWithErrorCheck(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
    // m_numberOfStops - 1 is the number of quads we will draw and
    // each quad will be split into two triangles, each of which will have three indices
    vector<GLuint> faces((m_numberOfStops - 1) * 2 * 3);
    faces[0] = 0; faces[1] = 4;  faces[2] = 3;
    faces[3] = 0; faces[4] = 4;  faces[5] = 1;
    faces[6] = 1; faces[7] = 4;  faces[8] = 5;
    faces[9] = 1; faces[10] = 5; faces[11] = 2;
    glCallWithErrorCheck(glBufferData,
                         GL_ELEMENT_ARRAY_BUFFER,
                         faces.size() * sizeof(GLuint),
                         faces.data(),
                         GL_STATIC_DRAW);
    m_numberOfConnectivityEntries = faces.size();
}

void GradientBackground::generateSphericalGradient() {

    // TODO: The geometry doesn't consider the aspect ratio of the window, it gets scaled unnaturally

    // Geometry for spherical gradient is composed of an inner disk and an outer annulus
    // The outer circle circumscribes a unit square, so it's radius R is sqrt(2)
    // The radius of the inner disk is R/3 and the thickness of the annulus is 2R/3
    constexpr byte numberOfSubdivisions = 16;
    float const diskRadius = std::sqrt(2.f)/3.f;
    float const annulusThickness = 2*diskRadius;
    auto numVertices = 2 *                          // Two circles
                     numberOfSubdivisions +         // Number of vertices per circle
                     1;                             // Vertex at center for the disk
    using Vertices = Array<float, 3>;
    Vertices vertices(2*numVertices);    // Position + color

    vector<unsigned> faces;
    auto numFaces = numberOfSubdivisions +          // Inner disk has number of subdivision triangles
                    numberOfSubdivisions * 2;       // Annulus has number of subdivision quads each of which is split into
                                                    // two triangles
    faces.reserve(3 * numFaces);                 // Each triangle has three index entries


    Color centerColor = ConfigurationReader::getInstance().getColor("RadialGradientCenterColor");
    Color innerColor = ConfigurationReader::getInstance().getColor("RadialGradientInnerColor");
    Color outerColor = ConfigurationReader::getInstance().getColor("RadialGradientOuterColor");

    // Create a circle centered at origin for the given radius
    auto generateCircle = [&centerColor](
        float const radius,
        Vertices& vertices,
        bool const asDisk,
        Color const& color){

        if (asDisk) {
            vertices.append(0.f, 0.f, 0.f);
            vertices.append(centerColor.x, centerColor.y, centerColor.z);
        }
        constexpr float thetaIncrement = (2 * M_PI)/numberOfSubdivisions;
        float theta = 0.f;
        for (byte i = 0; i < numberOfSubdivisions; ++i, theta+=thetaIncrement) {
            vertices.append(radius * cos(theta),
                            radius * sin(theta),
                            0.f);
            vertices.append(color.x, color.y, color.z);
        }
    };

    // Generate inner disk
    generateCircle(diskRadius, vertices, true, innerColor);
    for (unsigned i = 1; i <= numberOfSubdivisions; ++i) {
        faces.push_back(0);
        faces.push_back(i);
        faces.push_back(i == numberOfSubdivisions ? 1 : i+1);
    }

    // Generate annulus
    generateCircle(diskRadius + annulusThickness, vertices, false, outerColor);
    for (unsigned i = 1; i <= numberOfSubdivisions; ++i) {
        faces.push_back(i);
        faces.push_back(i + numberOfSubdivisions);
        faces.push_back(i == numberOfSubdivisions ? i + 1 : i + numberOfSubdivisions + 1);
        faces.push_back(i);
        faces.push_back(i == numberOfSubdivisions ? i + 1 : i + numberOfSubdivisions + 1);
        faces.push_back(i == numberOfSubdivisions ? 1 : i + 1);
    }

    // Upload vertex data to the gpu
    glCallWithErrorCheck(glBufferData,
                         GL_ARRAY_BUFFER,
                         6 * sizeof(GLfloat) * numVertices,
                         vertices.getData(),
                         GL_STATIC_DRAW);

    // Upload connectivity data to the gpu
    glCallWithErrorCheck(glGenBuffers, 1, &elementBufferObject);
    glCallWithErrorCheck(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
    glCallWithErrorCheck(glBufferData,
                         GL_ELEMENT_ARRAY_BUFFER,
                         sizeof(GLuint) * faces.size(),
                         faces.data(),
                         GL_STATIC_DRAW);
    m_numberOfConnectivityEntries = faces.size();
}

}
