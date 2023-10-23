#include "ArcballSphere.h"
#include "3dmath/primitives/Sphere.h"
#include "Types.h"

using namespace mv::common;

namespace mv::objects {

    // TODO: Read colors and other immutable properties from config
    ArcballSphere::ArcballSphere(common::DisplayDimensions const& displayDimensions)
    : ArcballVisualizationItem(displayDimensions, "ArcballSphere.vert", "ArcballSphere.frag")
    , resolution(64)
    , color({0.5, 0.5, 0.5}) {
        opacity = 0.2f;
    }

    void ArcballSphere::render() {
        ArcballVisualizationItem::render();

        glCallWithErrorCheck(glUseProgram, shaderProgram);

        glCallWithErrorCheck(glBindVertexArray, vertexArrayObject);
        glCallWithErrorCheck(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);

        glCallWithErrorCheck(glDisable, GL_DEPTH_TEST);
        glCallWithErrorCheck(glEnable, GL_BLEND);
        glCallWithErrorCheck(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glCallWithErrorCheck(glPolygonMode, GL_FRONT_AND_BACK, GL_FILL);
        glCallWithErrorCheck(glDrawElements, GL_TRIANGLES,
                             numConnectivityEntries,   // Number of entries in the connectivity array
                             GL_UNSIGNED_INT,          // Type of element buffer data
                             nullptr);                 // Offset into element buffer data
        // Reset state
        glCallWithErrorCheck(glEnable, GL_DEPTH_TEST);
        glCallWithErrorCheck(glDisable, GL_BLEND);
    }

    void ArcballSphere::generateRenderData() {
        if (readyToRender) {
            return;
        }
        // Create shader program
        createShaderProgram();
        glCallWithErrorCheck(glUseProgram, shaderProgram);

        // Create sphere centered at origin and set the radius to 1 so that
        // the sphere fits within the NDC view volume which is a cube of length 2
        math3d::Sphere sphere {{0, 0, 0}, 1.f, resolution };
        sphere.generateGeometry();

        // Create vertex array object
        glCallWithErrorCheck(glGenVertexArrays, 1, &vertexArrayObject);
        glCallWithErrorCheck(glBindVertexArray, vertexArrayObject);

        // Create vertex buffer object
        GLuint vbo;
        glCallWithErrorCheck(glGenBuffers, 1, &vbo);
        glCallWithErrorCheck(glBindBuffer, GL_ARRAY_BUFFER, vbo);

        // Upload sphere vertices to GPU
        size_t dataSize, offset;
        {
            size_t numVertices = sphere.getVertices().size();
            size_t numCoordinates = numVertices * 3;
            dataSize = numCoordinates * sizeof(GLfloat);
            std::unique_ptr<GLfloat[], void (*)(GLfloat const*)>
                vertexData(new GLfloat[numCoordinates],
                           [](GLfloat const* arrayData) { delete[] arrayData; });
            // NOTE: math3d::types::Vertex is not a 3-tuple of floats.
            // This forces us to copy data into a temporary buffer and pass that to OpenGL
            offset = 0;
            for (auto &vertex: sphere.getVertices()) {
                memcpy(vertexData.get() + offset, vertex.getData(), sizeof(GLfloat)*3);
                offset += 3;
            }
            glCallWithErrorCheck(glBufferData, GL_ARRAY_BUFFER, dataSize, vertexData.get(), GL_STATIC_DRAW);
        }

        // Define layout of the vertex data
        // NOTE: Sphere's vertices are in camera coordinates
        auto vertexAttribCoordinates = glGetAttribLocation(shaderProgram, "sphereVertex");
        glCallWithErrorCheck(glEnableVertexAttribArray, vertexAttribCoordinates);
        glCallWithErrorCheck(glVertexAttribPointer,
                             vertexAttribCoordinates, // The attribute being set
                             3,                       // Number of values for this attribute
                             GL_FLOAT,                // Data type
                             GL_FALSE,                // Normalization status
                             sizeof(GLfloat) * 3,     // Stride
                             nullptr);                // Offset into the array buffer data

        // Create element buffer object
        glCallWithErrorCheck(glGenBuffers, 1, &elementBufferObject);
        glCallWithErrorCheck(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);

        // Upload connectivity data to element buffer object
        static_assert(sizeof(math3d::types::Tri) == 12, "Memory layout for math3d::types::Tri is not compatible with the code on this platform");
        dataSize = sphere.getTris().size() * sizeof(math3d::types::Tri);
        glCallWithErrorCheck(glBufferData, GL_ELEMENT_ARRAY_BUFFER, dataSize, sphere.getTris().data(), GL_STATIC_DRAW);

        numConnectivityEntries = static_cast<unsigned>(sphere.getTris().size() * 3);

        generateColors();
    }

    void ArcballSphere::generateColors() {
        if (readyToRender) return;
        updateColor();
    }

    void ArcballSphere::fadeOut() {
        // TODO: Read this fade out amount from config
        opacity -= .02f;
        updateColor();
    }

    void ArcballSphere::updateColor() {
        auto sphereColorId = glCallWithErrorCheck(glGetUniformLocation, shaderProgram, "sphereColor");
        RGBAColor color4;
        color4.r = color.r;
        color4.g = color.g;
        color4.b = color.b;
        color4.a = opacity;
        glCallWithErrorCheck(glUniform4fv, sphereColorId, 1, color4.getData());
    }
}
