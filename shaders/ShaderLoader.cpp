#include "ShaderLoader.h"
#include <fstream>
#include <string>
#include <exception>
#include <filesystem>
using namespace std;

namespace mv {

void ShaderLoader::loadShader(std::string const& fileName, std::string& fileContents) {

    // Open file and seek to the end
    ifstream ifs(shaderDirectory/fileName, ios_base::ate /*seek to end of the file*/);
    if (!ifs) {
        throw std::runtime_error("Unable to read shader file: " + fileName);
    }

    // Get file size and allocate memory to hold the file's contents
    auto size = ifs.tellg();
    fileContents.resize(size);

    // Read file contents into output string
    ifs.seekg(0, ios_base::beg);
    ifs.read(&fileContents.at(0), size);
    ifs.close();
}

GLuint ShaderLoader::compileShader(GLuint const shaderId, std::string& compilerOutput) {

    // Compile shader and return compilation status
    glCallWithErrorCheck(glCompileShader, shaderId);
    GLint status{GL_TRUE};
    glCallWithErrorCheck(glGetShaderiv, shaderId, GL_COMPILE_STATUS, &status);
    compilerOutput.resize(outputSize);
    glCallWithErrorCheck(glGetShaderInfoLog, shaderId, outputSize, nullptr /*length as output*/, &compilerOutput.at(0));
    return status;
}


std::tuple<bool, GLuint> ShaderLoader::createShader(std::string const& fileName, std::string& compilerOutput, bool const isVertexShader) {
    string shaderContents;
    loadShader(fileName, shaderContents);

    // Create shader
    GLuint shaderId = glCallWithErrorCheck(glCreateShader, isVertexShader ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
    const char* shaderSrc = shaderContents.data();
    glCallWithErrorCheck(glShaderSource, shaderId, 1 /*number of shaders in the next string argument*/,
                         &shaderSrc /* double-pointer to array of strings*/,
                         nullptr /* array of string lengths, can be null*/);

    // Compile and return status
    return make_tuple(compileShader(shaderId, compilerOutput) == GL_TRUE, shaderId);
}

tuple<bool, GLuint> ShaderLoader::loadVertexShader(std::string const& fileName, std::string& compilerOutput) {
    return createShader(fileName, compilerOutput, true);
}

tuple<bool, GLuint> ShaderLoader::loadFragmentShader(std::string const& fileName, std::string& compilerOutput) {
    return createShader(fileName, compilerOutput, false);
}

}
