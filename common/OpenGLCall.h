#pragma once
#ifndef EMSCRIPTEN
#include "GL/glew.h"
#else
#include "GLES3/gl3.h"
#define GL_GLEXT_PROTOTYPES
#include "GLES3/gl2ext.h"
#define glBindVertexArray       glBindVertexArrayOES
#define glGenVertexArrays       glGenVertexArraysOES
#define glDeleteVertexArrays    glDeleteVertexArraysOES
#endif
#include <string>
#include "Environment.h"

// Macros and structs to wrap OpenGL calls in order to:
//
// 1. Make them noops when tests are run
// 2. Check errors after each call in debug mode

namespace mv {

static std::string getGLErrorString(GLint glError) {
    std::string errStr;
    switch (glError) {
        case GL_INVALID_ENUM:
            errStr = "Invalid enum";
            break;
        case GL_INVALID_VALUE:
            errStr = "Invalid value";
            break;
        case GL_INVALID_OPERATION:
            errStr = "Invalid operation";
            break;
        default:
            errStr = std::to_string(glError);
    }
    return errStr;
}

static GLuint glError = 0;
#if defined(DEBUG) && !defined(EMSCRIPTEN)
#define checkGLError(glFunc) \
if (!mv::common::Environment{}.isUnitTestingInProgress()) {               \
    glError = glGetError();                                               \
    if (glError) {                                                        \
        std::cout << #glFunc << " returned " << getGLErrorString(glError) \
             << " at line " << __LINE__ << " of "                         \
             << __FILE__ << std::endl;                                    \
        std::terminate();                                                 \
    }                                                                     \
}
#else
#define checkGLError(glFunc)
#endif

struct GLCallWrapper {
    template<typename FunctionPointer, typename... ArgumentTypes>
    auto operator()(FunctionPointer glFunction, ArgumentTypes&&... glArguments) {
        using ReturnType = decltype(glFunction(std::forward<ArgumentTypes>(glArguments)...));
        if (!mv::common::Environment{}.isUnitTestingInProgress()) {
            if constexpr (std::is_void_v<ReturnType>) {
                glFunction(std::forward<ArgumentTypes>(glArguments)...);
            } else {
                return glFunction(std::forward<ArgumentTypes>(glArguments)...);
            }
        } else if constexpr (!std::is_void_v<ReturnType>) {
                return ReturnType{};
        }
    }
};

#define glCallWithErrorCheck(glFunc, glArgs...) \
GLCallWrapper{}(glFunc, glArgs);                \
checkGLError(glFunc)
}
