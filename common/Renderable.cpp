#include "Renderable.h"
#include "ShaderLoader.h"
#include "Camera.h"

#include <filesystem>
using namespace std;

namespace meshviewer {

Renderable::Renderable(const std::string& vertexShaderFileName,
                       const std::string& fragmentShaderFileName)
    : m_vertexShaderFileName(vertexShaderFileName)
    , m_fragmentShaderFileName(fragmentShaderFileName)
    , m_readyToRender(false) {

}

void Renderable::createShaderProgram() {

    filesystem::path shaderDir = "./shaders";

    // Vertex Shader
    string compilerOut;
    auto status = meshviewer::ShaderLoader().loadVertexShader(shaderDir/m_vertexShaderFileName, compilerOut);
    if (!get<0>(status)) {
        throw std::runtime_error(compilerOut.data());
    }
    GLuint vertexShaderId = get<1>(status);

    // Fragment Shader
    status = meshviewer::ShaderLoader().loadFragmentShader(shaderDir/m_fragmentShaderFileName, compilerOut);
    if (!get<0>(status)) {
        throw std::runtime_error(compilerOut.data());
    }
    GLuint fragmentShaderId = get<1>(status);

    // Create shader program
    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vertexShaderId);
    glAttachShader(m_shaderProgram, fragmentShaderId);
    glBindFragDataLocation(m_shaderProgram, 0, "fragmentColor");

    // Link program
    glLinkProgram(m_shaderProgram);

    // Check the program
    GLint result;
    int infoLogLength;
	glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &result);
	glGetProgramiv(m_shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( infoLogLength > 0 ){
		std::unique_ptr<char> pProgramErrorMessage {new char[infoLogLength+1]};
		glGetProgramInfoLog(m_shaderProgram, infoLogLength, NULL, pProgramErrorMessage.get());
		cerr << pProgramErrorMessage.get();
        throw std::runtime_error("Failed to load shaders " + std::string(pProgramErrorMessage.get()));
	}
}

// Precondition: Every vertex shader must have a uniform mat4 variable named
// modelViewProjectionTransform
void Renderable::setModelViewProjection(Camera const& camera) {

    // Compute the combined transform that will transform a vertex from
    // world coordinates to normalized view coordinates
    // The matrix multiplication order is the reverse order of actual transformations
    // Object -> Global, Global -> Camera, Camera -> Homogenous Coordinates
    // TODO: Setup an octave session and observe the conversion to homogenous coordinates
    glm::mat4 compositeTransform = camera.getProjectionTransform() *
                                   camera.getViewTransform() *
                                   getModelTransform();

    // Get the composite transform matrix id in the shader
    GLuint mvpId = glGetUniformLocation(m_shaderProgram, "modelViewProjectionTransform");

    // Set model view projection projection
    glUniformMatrix4fv(mvpId,
                       1,        // num matrices,
                       GL_FALSE, // transpose
                       &compositeTransform[0][0]);

}

// Precondition: Every vertex shader must have a uniform mat4 variable named
// modelViewProjectionTransform
void Renderable::setModelView(Camera const& camera) {

    // Set model view
    GLuint mvId = glGetUniformLocation(m_shaderProgram, "modelViewTransform");
    glm::mat4 modelViewTransform = camera.getViewTransform() * getModelTransform();
    glUniformMatrix4fv(mvId,
                       1,
                       GL_FALSE,
                       &modelViewTransform[0][0]);
}

}
