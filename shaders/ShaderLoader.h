#pragma once

#include "Types.h"
#include "MeshViewerObject.h"
#include <string>
#include <tuple>

namespace mv {

    class ShaderLoader : public MeshViewerObject {
        public:
            ShaderLoader() : m_outputSize(1024) {}
        public:
            std::tuple<bool, GLuint> loadVertexShader(const std::string& shaderFileName, std::string& compilerOutput);
            std::tuple<bool, GLuint> loadFragmentShader(const std::string& shaderFileName, std::string& compilerOutput);
        private:
            void loadShader(const std::string& shaderFileName, std::string& fileContents);
            GLuint compileShader(const GLuint shaderId, std::string& compilerOutput); 
        private:
            const unsigned m_outputSize;
    };
}