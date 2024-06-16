#include "Drawable.h"
#include "ShaderLoader.h"

#include <filesystem>
#include <utility>
#include "ShaderLoaderFactory.h"
using namespace std;

namespace mv {

using namespace common;

Drawable::Drawable(std::string  vertexShaderFileName, std::string fragmentShaderFileName,
                   Camera::SharedCameraPointer camera, Effect supportedEffects)
    : vertexShaderFileName(std::move(vertexShaderFileName))
    , fragmentShaderFileName(std::move(fragmentShaderFileName))
    , camera(camera)
    , supportedEffects(supportedEffects) {

}

void Drawable::createShaderProgram() {

    // Vertex Shader
    string compilerOut;
    auto status = ShaderLoaderFactory::getInstance().getShaderLoader().loadVertexShader(vertexShaderFileName, compilerOut);
    if (!get<0>(status)) {
        throw std::runtime_error(compilerOut.data());
    }
    GLuint vertexShaderId = get<1>(status);

    // Fragment Shader
    status = ShaderLoaderFactory::getInstance().getShaderLoader().loadFragmentShader(fragmentShaderFileName, compilerOut);
    if (!get<0>(status)) {
        throw std::runtime_error(compilerOut.data());
    }
    GLuint fragmentShaderId = get<1>(status);

    // Create shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShaderId);
    glAttachShader(shaderProgram, fragmentShaderId);
#ifndef EMSCRIPTEN
    glBindFragDataLocation(shaderProgram, 0, "fragmentColor");
#endif

    // Link program
    glLinkProgram(shaderProgram);

    // Check the program status
    GLint result;
    int infoLogLength;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
	glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( result == GL_FALSE ) {
        std::unique_ptr<char> errorMessage;
        if (infoLogLength > 0) {
            errorMessage.reset(new char[infoLogLength + 1]);
            glGetProgramInfoLog(shaderProgram, infoLogLength, NULL, errorMessage.get());
        }
		std::cerr << "Failed to create shader program for "
             << vertexShaderFileName << " and " << fragmentShaderFileName << ". ";
        if (errorMessage) {
            std::cerr << "glLinkProgram returned " << errorMessage.get() << endl;
        }
        throw std::runtime_error("Failed to load shaders ");
	}
}

}
