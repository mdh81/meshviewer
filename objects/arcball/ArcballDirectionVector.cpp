
#include "ArcballDirectionVector.h"
#include "Texture.h"
#include "3dmath/primitives/Plane.h"
#include "3dmath/ScalingMatrix.h"

using namespace mv::common;

namespace mv::objects {

    ArcballDirectionVector::ArcballDirectionVector(DisplayDimensions const& displayDimensions)
            : ArcballVisualizationItem(displayDimensions, "ArcballAxis.vert", "ArcballAxis.frag")
            , lineWidthPixels(10)
            , pointOnSphere{1.f, 0.f, 0.f} {
        ArcballVisualizationItem::color = {.4f, .4f, .4f, .7f };
    }

    void ArcballDirectionVector::render() {
        if (!readyToRender) {
            generateRenderData();
            readyToRender = true;
        }

        glCallWithErrorCheck(glUseProgram, shaderProgram);

        glCallWithErrorCheck(glBindVertexArray, vertexArrayObject);
        glCallWithErrorCheck(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);

        glCallWithErrorCheck(glDisable, GL_DEPTH_TEST);
        glCallWithErrorCheck(glEnable, GL_BLEND);
        glCallWithErrorCheck(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Draw quad for the shaft
        glCallWithErrorCheck(glActiveTexture, GL_TEXTURE0);
        glCallWithErrorCheck(glBindTexture, GL_TEXTURE_2D, quadTextureId);

        glCallWithErrorCheck(glPolygonMode, GL_FRONT_AND_BACK, GL_FILL);
        glCallWithErrorCheck(glDrawElements, GL_TRIANGLES,
                             numConnectivityEntriesQuad,   // Number of entries in the connectivity array
                             GL_UNSIGNED_INT,              // Type of element buffer data
                             nullptr);                     // Offset into element buffer data

        // Draw tri for the vector head
        glCallWithErrorCheck(glBindTexture, GL_TEXTURE_2D, triTextureId);
        glCallWithErrorCheck(glDrawElements, GL_TRIANGLES,
                             numConnectivityEntriesTri,         // Number of entries in the connectivity array
                             GL_UNSIGNED_INT,                   // Type of element buffer data
                             (void*)(numConnectivityEntriesQuad * sizeof(GL_UNSIGNED_INT)));     // Offset into element buffer data

        setTransforms();

        // Reset state
        glCallWithErrorCheck(glEnable, GL_DEPTH_TEST);
        glCallWithErrorCheck(glDisable, GL_BLEND);
    }

    void ArcballDirectionVector::createTexture(bool const isQuadTexture) {
        std::string const textureFileName = isQuadTexture ? "axis.png" : "arcball_vector_head.png";
        unsigned& textureId = isQuadTexture ? quadTextureId : triTextureId;
        auto texture = Texture(textureFileName);
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
        glCallWithErrorCheck(glGenerateMipmap, GL_TEXTURE_2D);
    }

    void ArcballDirectionVector::generateRenderData() {
        if (readyToRender) {
            return;
        }

        // TODO: On regenerate, delete previous textures and GPU buffers

        // Create shader program
        createShaderProgram();
        glCallWithErrorCheck(glUseProgram, shaderProgram);

        // Create an arrow glpyh. The arrow geometry is in camera coordinates and is of unit length
        // It will be transformed to the correct position and orientation in setTransforms()
        TwoDPoints vertices;
        math3d::types::Tris tris;
        constructGeometry(vertices, tris, getScaleFactorInCameraSpace(lineWidthPixels));

        // Load textures
        createTexture(/*isQuadTexture=*/ true);
        createTexture(/*isQuadTexture=*/ false);

        // Create vertex array object
        glCallWithErrorCheck(glGenVertexArrays, 1, &vertexArrayObject);
        glCallWithErrorCheck(glBindVertexArray, vertexArrayObject);

        // Create vertex buffer object
        GLuint vbo;
        glCallWithErrorCheck(glGenBuffers, 1, &vbo);
        glCallWithErrorCheck(glBindBuffer, GL_ARRAY_BUFFER, vbo);

        // Upload vector glyph's vertices and texture coordinates to GPU
        size_t dataSize;
        {
            size_t numVertices = vertices.size();
            size_t numFloats = numVertices * (3 /* position */ + 2 /* texture */);
            dataSize = numFloats * sizeof(GLfloat);
            std::unique_ptr<GLfloat[], void (*)(GLfloat const*)>
                    vertexData(new GLfloat[numFloats],
                               [](GLfloat const* arrayData) { delete[] arrayData; });
            unsigned index = 0;
            // Apply 1D texture in axis.png to the quad. Apply the 2D texture in
            // arcball_vector_head.png to the triangle that forms the head of the vector
            std::array<Point2D, 7> textureCoords = { Point2D{0, 0},   // 1D texture for line
                                                     Point2D{0, 0},
                                                     Point2D{0, 1},
                                                     Point2D{0, 1},
                                                     Point2D{1, 0.5}, // 2D texture for tri
                                                     Point2D{0, 0.65},
                                                     Point2D{0, 0.35},
                                                   };
            for (auto i = 0u; i < 7; ++i) {
                // position
                vertexData.get()[index++] = vertices.at(i).x;
                vertexData.get()[index++] = vertices.at(i).y;
                vertexData.get()[index++] = 0;
                // texture
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
        dataSize = tris.size() * sizeof(math3d::types::Tri);
        glCallWithErrorCheck(glBufferData, GL_ELEMENT_ARRAY_BUFFER, dataSize, tris.data(), GL_STATIC_DRAW);

        constexpr unsigned numTrisInShaftQuad = 2;
        numConnectivityEntriesQuad = static_cast<unsigned>(numTrisInShaftQuad * 3);
        numConnectivityEntriesTri = static_cast<unsigned>((tris.size() - numTrisInShaftQuad) * 3);

        generateColors();
    }

    void ArcballDirectionVector::generateColors() {
        if (readyToRender) return;
        GLint axisColorId = glCallWithErrorCheck(glGetUniformLocation, shaderProgram, "axisColor");
        glCallWithErrorCheck(glUniform4fv, axisColorId, 1, color.getData());
    }

    void ArcballDirectionVector::fadeOut() {

    }

    void ArcballDirectionVector::setTransforms() {
        // Allow the base class to set the orthographic projection
        ArcballVisualizationItem::setTransforms();
        GLint axisTransformId = glCallWithErrorCheck(glGetUniformLocation, shaderProgram, "axisTransformMatrix");

        // Project the point on the sphere to the XY plane
        Point3D projectedSpherePoint = {pointOnSphere.x, pointOnSphere.y, 0};

        // Scale along x (the original axis of the vector geometry) so the vector's magnitude
        math3d::ScalingMatrix<float> scale {static_cast<float>(projectedSpherePoint.length()), 1, 1};

        // Orient the vector
        // x-axis of the vector is along the vector from origin to the projected sphere point
        // z-axis will be the plane normal
        // y-axis will the plane vector perpendicular to x and z

        auto axisTransform = math3d::IdentityMatrix<float, 4, 4>{};
        auto directionVector = projectedSpherePoint.normalize();
        Vector3D viewNormal {0, 0, 1};
        axisTransform[0] = math3d::Vector4<float> {directionVector, 0};
        axisTransform[1] = math3d::Vector4<float> {(directionVector * viewNormal).normalize(), 0};
        axisTransform[2] = math3d::Vector4<float> { viewNormal, 0};

        auto finalTransform = scale * axisTransform;

        glCallWithErrorCheck(glUniformMatrix4fv,
                             axisTransformId,
                             1,                 // num matrices,
                             GL_TRUE,           // transpose
                             finalTransform);   // Matrix to pointer conversion operator call
    }

    void ArcballDirectionVector::constructGeometry(TwoDPoints &vertices, math3d::types::Tris& tris, float lineWidthCamera) {

        // Create a quad in camera coordinates. The dimensions are shaft width x line width
        auto shaftWidth = 0.98f;
        auto halfLineWidth = lineWidthCamera*0.5f;
        vertices.push_back({0, -halfLineWidth});
        vertices.push_back({shaftWidth, -halfLineWidth});
        vertices.push_back({shaftWidth, halfLineWidth});
        vertices.push_back({0, halfLineWidth});

        tris.push_back({0, 1, 2});
        tris.push_back({0, 2, 3});

        // Create two tris in camera coordinates for the vector's tip.
        // The dimensions are head width * line width
        auto headWidth = 1-shaftWidth;
        vertices.push_back({1, 0});
        vertices.push_back({1-headWidth, lineWidthCamera*1.25f});
        vertices.push_back({1-headWidth, -lineWidthCamera*1.25f});

        tris.push_back({4, 5, 2});
        tris.push_back({1, 4, 2});
        tris.push_back({6, 4, 1});
    }
}