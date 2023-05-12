#include "Renderable.h"
#include "ShaderLoader.h"

#include <filesystem>
#include <utility>
using namespace std;

namespace mv {

using namespace common;

Renderable::Renderable(std::string  vertexShaderFileName, std::string  fragmentShaderFileName, Effect supportedEffects)
    : vertexShaderFileName(std::move(vertexShaderFileName))
    , fragmentShaderFileName(std::move(fragmentShaderFileName))
    , camera(Camera(*this, Camera::ProjectionType::Perspective))
    , supportedEffects(supportedEffects) {

}

void Renderable::createShaderProgram() {

    filesystem::path shaderDir = "./shaders";

    // Vertex Shader
    string compilerOut;
    auto status = mv::ShaderLoader().loadVertexShader(shaderDir / vertexShaderFileName, compilerOut);
    if (!get<0>(status)) {
        throw std::runtime_error(compilerOut.data());
    }
    GLuint vertexShaderId = get<1>(status);

    // Fragment Shader
    status = mv::ShaderLoader().loadFragmentShader(shaderDir / fragmentShaderFileName, compilerOut);
    if (!get<0>(status)) {
        throw std::runtime_error(compilerOut.data());
    }
    GLuint fragmentShaderId = get<1>(status);

    // Create shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShaderId);
    glAttachShader(shaderProgram, fragmentShaderId);
    glBindFragDataLocation(shaderProgram, 0, "fragmentColor");

    // Link program
    glLinkProgram(shaderProgram);

    // Check the program
    GLint result;
    int infoLogLength;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
	glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( infoLogLength > 0 ){
		std::unique_ptr<char> pProgramErrorMessage {new char[infoLogLength+1]};
		glGetProgramInfoLog(shaderProgram, infoLogLength, NULL, pProgramErrorMessage.get());
		cerr << pProgramErrorMessage.get();
        throw std::runtime_error("Failed to load shaders " + std::string(pProgramErrorMessage.get()));
	}
}

// Precondition: Every vertex shader must have a uniform mat4 variables named
// modelViewTransform and projectionTransform
void Renderable::setTransforms() {

    // TODO: Use mathlib's matrix

    // Compute the transform that will transform a vertex from
    // world coordinates to camera coordinates
    // The matrix multiplication order is the reverse order of actual transformations
    // Object -> Global, Global -> Camera, Camera -> Homogenous Coordinates
    glm::mat4 modelView = camera.getViewTransform() * getModelTransform();

    // Get the model view transform matrix id in the shader
    GLint modelViewId = glGetUniformLocation(shaderProgram, "modelViewTransform");

    // Set model view
    glUniformMatrix4fv(modelViewId,
                       1,        // num matrices,
                       GL_FALSE, // transpose
                       &modelView[0][0]);
    // Get the model view transform matrix id in the shader
    GLint projectionId = glGetUniformLocation(shaderProgram, "projectionTransform");

    // Set projection
    glUniformMatrix4fv(projectionId,
                       1,        // num matrices,
                       GL_FALSE, // transpose
                       &camera.getProjectionTransform()[0][0]);
}

}
