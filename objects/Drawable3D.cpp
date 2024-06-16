#include "Drawable3D.h"

namespace mv {

    Drawable3D::Drawable3D(std::string vertexShaderFileName, std::string fragmentShaderFileName,
                           Effect supportedEffects)
    : Drawable(vertexShaderFileName, fragmentShaderFileName, nullptr, supportedEffects) {}

    // Precondition: Every vertex shader must have a uniform mat4 variables named
    // modelViewTransform and projectionTransform
    void Drawable3D::setTransforms() {

        // TODO 1: Use mathlib's matrix

        // TODO 2: Only update model view transform when camera is dirty

        // Compute the transform that will transform a vertex from
        // world coordinates to camera coordinates
        // The matrix multiplication order is the reverse order of actual transformations
        // Object -> Global, Global -> Camera, Camera -> Homogenous Coordinates
        glm::mat4 modelView = camera->getViewTransform() * getModelTransform();

        // Get the model view transform matrix id in the shader
        GLint modelViewId = glGetUniformLocation(shaderProgram, "modelViewTransform");

        // Set model view
        glUniformMatrix4fv(modelViewId,
                           1,        // num matrices,
                           GL_FALSE, // transpose
                           &modelView[0][0]);

        if (needsProjectionUpdate()) {
            // Get the projection transform matrix id in the shader
            GLint projectionId = glGetUniformLocation(shaderProgram, "projectionTransform");

            // Set projection
            glUniformMatrix4fv(projectionId,
                               1,        // num matrices,
                               GL_FALSE, // transpose
                               &camera->getProjectionTransform()[0][0]);

            setProjectionUpdated();
        }
    }

} // mv

