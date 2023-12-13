#pragma once

#ifdef EMSCRIPTEN

#include "ShaderLoader.h"

namespace mv {

    class EmscriptenShaderLoader : public ShaderLoader {
    protected:
        virtual void loadShader(const std::string &shaderFileName, std::string &fileContents);

    };

}

#endif