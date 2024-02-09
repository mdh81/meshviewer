#pragma once

#include "Types.h"
#include "MeshViewerObject.h"
#include <string>
#include <tuple>

namespace mv {

    class ShaderLoader : public MeshViewerObject {
        public:
            std::tuple<bool, GLuint> loadVertexShader(std::string const& shaderFileName, std::string& compilerOutput);
            std::tuple<bool, GLuint> loadFragmentShader(std::string const& shaderFileName, std::string& compilerOutput);
            virtual ~ShaderLoader() = default;
        protected:
            virtual void loadShader(std::string const& shaderFileName, std::string& fileContents);
            GLuint compileShader(GLuint const shaderId, std::string& compilerOutput);
            std::tuple<bool, GLuint> createShader(std::string const& fileName, std::string& compilerOutput, bool const isVertexShader);
        private:
            unsigned const outputSize {1024};
            std::filesystem::path const shaderDirectory {"./shaders"};
    };
}