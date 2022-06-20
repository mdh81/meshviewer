#ifndef MESH_VIEWER_SHADER_LOADER_H
#define MESH_VIEWER_SHADER_LOADER_H

#include "MeshViewerObject.h"

#include "GL/glew.h"
#include <string>
#include <tuple>

namespace meshviewer {

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

#endif
