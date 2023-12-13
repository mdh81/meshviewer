#pragma once

#include <memory>
#include "ShaderLoader.h"
#include "EmscriptenShaderLoader.h"

namespace mv {

    class ShaderLoaderFactory {

    public:
        static ShaderLoaderFactory& getInstance() {
            static ShaderLoaderFactory instance{};
            return instance;
        }

        ShaderLoader& getShaderLoader() {
            if (!shaderLoader) {
#ifdef EMSCRIPTEN
                shaderLoader.reset(new EmscriptenShaderLoader{});
#else
                shaderLoader.reset(new ShaderLoader());
#endif
            }
            return *shaderLoader.get();
        }

    private:
        std::unique_ptr<ShaderLoader> shaderLoader;
    };

}
