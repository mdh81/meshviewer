#include "ArcballVisualizationItem.h"
#include "Types.h"

using namespace mv::common;

namespace mv::objects {

    // TODO: Implement flyweight pattern to retain state common to all visualization items like display dimenions
    //  in a flyweight object
    ArcballVisualizationItem::ArcballVisualizationItem(common::DisplayDimensions const& displayDimensions, std::string const& vertexShaderFileName, std::string const& fragmentShaderFileName)
    : Drawable3D(vertexShaderFileName, fragmentShaderFileName)
    , displayDimensions(displayDimensions) {
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

    // TODO: Add derivation to docs
    float ArcballVisualizationItem::getScaleFactorInCameraSpace(unsigned short sizeInPixels) const {
        return  (*projectionMatrix)(1, 1) *
                (2*sizeInPixels)/(displayDimensions.normalizedViewportSize.y * displayDimensions.frameBufferHeight);

        // OR

        //return ((2*sizeInPixels)/(displayDimensions.normalizedViewportSize.x * displayDimensions.frameBufferWidth)) /
        //       (*projectionMatrix)(0, 0);


    }
}
