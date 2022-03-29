#include "ShaderLoader.h"
#include <fstream>
#include <string>
#include <exception>
using namespace std;

namespace meshviewer {

void ShaderLoader::loadShader(const std::string& fileName, std::string& fileContents) {
    // Open file and seek to the end
    ifstream ifs(fileName, ios_base::ate /*seek to end of the file*/);
    if (!ifs) {
        throw std::runtime_error("Unable to read vertex shader file: " + fileName);
    }
    
    // Get file size and allocate memory to hold the file's contents
    auto size = ifs.tellg();
    fileContents.resize(size);
    
    // Read file contents into output string
    ifs.seekg(0, ios_base::beg);
    ifs.read(&fileContents.at(0), size); 
    ifs.close();
}

GLuint ShaderLoader::compileShader(const GLuint shaderId, std::string& compilerOutput) {
    
    // Compile shader and return compilation status
    glCompileShader(shaderId);
    GLint status;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
    compilerOutput.resize(m_outputSize);
    glGetShaderInfoLog(shaderId, m_outputSize, nullptr /*length as output*/, &compilerOutput.at(0));
    return status;
}

tuple<bool, GLuint> ShaderLoader::loadVertexShader(const std::string& fileName, std::string& compilerOutput) {
    
    // Load vertex shader file contents into memory
    string vshaderStr;
    loadShader(fileName, vshaderStr); 
    
    // Create shader
    GLuint shaderId = glCreateShader(GL_VERTEX_SHADER);
    const char* shaderSrc = vshaderStr.data();
    glShaderSource(shaderId, 1 /*number of shaders in the next string argument*/,
                   &shaderSrc /* double-pointer to array of strings*/, 
                   nullptr /* array of string lengths, can be null*/); 
    
    // Compile and return status
    return make_tuple(compileShader(shaderId, compilerOutput) == GL_TRUE, shaderId);
}

tuple<bool, GLuint> ShaderLoader::loadFragmentShader(const std::string& fileName, std::string& compilerOutput) {
    
    // Load vertex shader file contents into memory
    string fshaderStr;
    loadShader(fileName, fshaderStr); 
    
    // Create shader
    GLuint shaderId = glCreateShader(GL_FRAGMENT_SHADER);
    const char* shaderSrc = fshaderStr.data();
    glShaderSource(shaderId, 1 /*number of shaders in the next string argument*/,
                   &shaderSrc /* double-pointer to array of strings*/, 
                   nullptr /* array of string lengths, can be null*/); 
    
     
    // Compile and return status
    return make_tuple(compileShader(shaderId, compilerOutput) == GL_TRUE, shaderId);
}

}
