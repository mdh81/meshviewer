#include "ArcballVisualizationItem.h"
#include "Types.h"

using namespace mv::common;

namespace mv::objects {

    ArcballVisualizationItem::ArcballVisualizationItem(std::string const& vertexShaderFileName, std::string const& fragmentShaderFileName)
            : Drawable3D(vertexShaderFileName, fragmentShaderFileName) {
        opacity = 1.f;
    }

    void ArcballVisualizationItem::notifyWindowResized(unsigned int windowWidth, unsigned int windowHeight) {
        Renderable::notifyWindowResized(windowWidth, windowHeight);
        updateProjectionMatrix();
    }

    void ArcballVisualizationItem::setInitialProjection(float aspectRatio) {
        this->aspectRatio = aspectRatio;
        updateProjectionMatrix();
    }

    void ArcballVisualizationItem::updateProjectionMatrix() {
        // Create an orthographic projection matrix that will render our sphere with the correct
        // aspect ratio.
        float height = 1.f;
        float width = height * aspectRatio;
        projectionMatrix.update({{-width, -height, -1.f}, {+width, +height, +1.f}});
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
                                 projectionMatrix.getData());
            setProjectionUpdated();
        }
    }
}
