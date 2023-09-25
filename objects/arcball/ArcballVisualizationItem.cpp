#include "ArcballVisualizationItem.h"
#include "Types.h"

using namespace mv::common;

namespace mv::objects {

    ArcballVisualizationItem::ArcballVisualizationItem(std::string const& vertexShaderFileName, std::string const& fragmentShaderFileName)
            : Drawable3D(vertexShaderFileName, fragmentShaderFileName) {
        opacity = 1.f;
    }

    void ArcballVisualizationItem::render() {
        if (!readyToRender) {
            generateRenderData();
            readyToRender = true;
        }
        setTransforms();
    }

    void ArcballVisualizationItem::setTransforms() {
        if (needsProjectionUpdate()) {
            GLint projectionId = glCallWithErrorCheck(glGetUniformLocation, shaderProgram, "orthographicProjectionMatrix");
            // Set projection
            glCallWithErrorCheck(glUniformMatrix4fv, projectionId,
                                 1,        // num matrices,
                                 GL_FALSE, // transpose
                                 projectionMatrix->getData());
            setProjectionUpdated();
        }
    }
}
