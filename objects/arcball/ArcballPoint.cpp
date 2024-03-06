#include "ArcballPoint.h"
#include "3dmath/primitives/Plane.h"
#include "3dmath/TranslationMatrix.h"
#include "3dmath/ScalingMatrix.h"
#include "Texture.h"

using namespace mv::common;

namespace mv::objects {

    // TODO: Read colors and other immutable properties from config
    ArcballPoint::ArcballPoint(common::DisplayDimensions const& displayDimensions)
    : ArcballVisualizationItem(displayDimensions, "ArcballPoint.vert", "ArcballPoint.frag")
    , scaleFactor(1)
    , pointSizePixels(20) {
        ArcballVisualizationItem::color = {1.f, 1.f, 1.f, 0.7f};
    }

    void ArcballPoint::render() {
        if (!readyToRender) {
            scaleFactor = getScaleFactorInCameraSpace(pointSizePixels);
            generateRenderData();
            readyToRender = true;
        }

        glCallWithErrorCheck(glUseProgram, shaderProgram);

        glCallWithErrorCheck(glBindVertexArray, vertexArrayObject);
        glCallWithErrorCheck(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);

        glCallWithErrorCheck(glDisable, GL_DEPTH_TEST);
        glCallWithErrorCheck(glEnable, GL_BLEND);
        glCallWithErrorCheck(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glCallWithErrorCheck(glActiveTexture, GL_TEXTURE0);
        glCallWithErrorCheck(glBindTexture, GL_TEXTURE_2D, textureId);

#ifndef EMSCRIPTEN
        glCallWithErrorCheck(glPolygonMode, GL_FRONT_AND_BACK, GL_FILL);
#endif
        glCallWithErrorCheck(glDrawElements, GL_TRIANGLES,
                             numConnectivityEntriesQuad,   // Number of entries in the connectivity array
                             GL_UNSIGNED_INT,          // Type of element buffer data
                             nullptr);                 // Offset into element buffer data
        setTransforms();

        // Reset state
        glCallWithErrorCheck(glEnable, GL_DEPTH_TEST);
        glCallWithErrorCheck(glDisable, GL_BLEND);
    }

    void ArcballPoint::generateRenderData() {
        if (readyToRender) {
            return;
        }

        // Load texture
        auto texture = Texture("arcball_intersection_point.png");
        glCallWithErrorCheck(glGenTextures, 1, &textureId);
        glCallWithErrorCheck(glBindTexture, GL_TEXTURE_2D, textureId);
        glCallWithErrorCheck(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glCallWithErrorCheck(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glCallWithErrorCheck(
                glTexImage2D,
                GL_TEXTURE_2D,      // target
                0,                  // level
                GL_RGBA,            // internal format
                texture.width,
                texture.height,
                0,                  // border
                GL_RGBA,            // format
                GL_UNSIGNED_BYTE,   // data type
                texture.data.get());// data
        glGenerateMipmap(GL_TEXTURE_2D);

        // Create shader program
        createShaderProgram();
        glCallWithErrorCheck(glUseProgram, shaderProgram);

        // Create a unit square in camera coordinates
        math3d::Plane plane {{0, 0, 0}, {0, 0, 1}, 1};
        plane.generateGeometry();

        // Create vertex array object
        glCallWithErrorCheck(glGenVertexArrays, 1, &vertexArrayObject);
        glCallWithErrorCheck(glBindVertexArray, vertexArrayObject);

        // Create vertex buffer object
        GLuint vbo;
        glCallWithErrorCheck(glGenBuffers, 1, &vbo);
        glCallWithErrorCheck(glBindBuffer, GL_ARRAY_BUFFER, vbo);

        // Upload square vertices and texture coordinates to GPU
        size_t dataSize;
        {
            size_t numVertices = plane.getVertices().size();
            size_t numCoordinates = numVertices * 3 * 2;
            dataSize = numCoordinates * sizeof(GLfloat);
            std::unique_ptr<GLfloat[], void (*)(GLfloat const*)>
                vertexData(new GLfloat[numCoordinates],
                           [](GLfloat const* arrayData) { delete[] arrayData; });
            unsigned index = 0;
            std::array<Point2D, 4> textureCoords = { Point2D {0, 0},
                                                     Point2D {1, 0},
                                                     Point2D {1, 1},
                                                     Point2D {0, 1} };
            for (auto i = 0u; i < 4; ++i) {
                vertexData.get()[index++] = plane.getVertices().at(i).x;
                vertexData.get()[index++] = plane.getVertices().at(i).y;
                vertexData.get()[index++] = plane.getVertices().at(i).z;
                vertexData.get()[index++] = textureCoords.at(i).x;
                vertexData.get()[index++] = textureCoords.at(i).y;
            }
            glCallWithErrorCheck(glBufferData, GL_ARRAY_BUFFER, dataSize, vertexData.get(), GL_STATIC_DRAW);
        }

        // Define layout of the vertex data
        auto vertexCoordinatesAttrib = glGetAttribLocation(shaderProgram, "vertexCoordinatesIn");
        glCallWithErrorCheck(glEnableVertexAttribArray, vertexCoordinatesAttrib);
        glCallWithErrorCheck(glVertexAttribPointer,
                             vertexCoordinatesAttrib, // The attribute being set
                             3,                       // Number of values for this attribute
                             GL_FLOAT,                // Data type
                             GL_FALSE,                // Normalization status
                             sizeof(GLfloat) * 5,     // Stride
                             nullptr);                // Offset into the array buffer data

        auto textureCoordinatesAttrib = glGetAttribLocation(shaderProgram, "textureCoordinatesIn");
        glCallWithErrorCheck(glEnableVertexAttribArray, textureCoordinatesAttrib);
        glCallWithErrorCheck(glVertexAttribPointer,
                             textureCoordinatesAttrib,
                             2,
                             GL_FLOAT,
                             GL_FALSE,
                             sizeof(GLfloat) * 5,
                             (void*)(sizeof(GLfloat) * 3));

        // Create element buffer object
        glCallWithErrorCheck(glGenBuffers, 1, &elementBufferObject);
        glCallWithErrorCheck(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);

        // Upload connectivity data to element buffer object
        static_assert(sizeof(math3d::types::Tri) == 12, "Memory layout for math3d::types::Tri is not compatible with the code on this platform");
        dataSize = plane.getTris().size() * sizeof(math3d::types::Tri);
        glCallWithErrorCheck(glBufferData, GL_ELEMENT_ARRAY_BUFFER, dataSize, plane.getTris().data(), GL_STATIC_DRAW);

        numConnectivityEntriesQuad = static_cast<unsigned>(plane.getTris().size() * 3);

        generateColors();
    }

    void ArcballPoint::generateColors() {
        if (readyToRender) return;
        GLint pointColorId = glCallWithErrorCheck(glGetUniformLocation, shaderProgram, "pointColor");
        glCallWithErrorCheck(glUniform4fv, pointColorId, 1, color.getData());
    }

    void ArcballPoint::fadeOut() {

    }

    void ArcballPoint::setTransforms() {
        // Allow the base class to set the orthographic projection
        ArcballVisualizationItem::setTransforms();
        GLint pointTransformId = glCallWithErrorCheck(glGetUniformLocation, shaderProgram, "pointTransformMatrix");
        math3d::Matrix<float, 4, 4> pointTransform =
                math3d::TranslationMatrix<float>{positionCamera } *
                math3d::ScalingMatrix<float>{scaleFactor, scaleFactor, 1};
        // Set point location and size
        glCallWithErrorCheck(glUniformMatrix4fv,
                             pointTransformId,
                             1,                 // num matrices,
                             GL_FALSE,          // transpose
                             pointTransform);   // Matrix to pointer conversion operator call
    }

}