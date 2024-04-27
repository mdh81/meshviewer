#ifdef EMSCRIPTEN

#include "EmscriptenShaderLoader.h"
#include "emscripten.h"
#include <cstdlib>
#include <filesystem>

void mv::EmscriptenShaderLoader::loadShader(std::string const& shaderFileName,
                                            std::string& fileContents) {

    // Load shader from vertex and fragment shader html elements created by the build system
    // when the emscripten build is configured. For each shader source file (with extension glsl)
    // the build system will create a DOM element with the same name as the source file.

    std::string shaderBaseName {std::filesystem::path{shaderFileName}.stem()};
    std::string shaderExtension {std::filesystem::path{shaderFileName}.extension().string().substr(1)};

    // Adjust shader name for those shaders that use the <shader>.vert/frag naming convention
    std::string shaderName = shaderBaseName;
    if (shaderExtension == "vert" || shaderExtension == "frag") {
        shaderName = shaderBaseName + '_' + shaderExtension;
    }

    // clang-format off
    char* shaderSource = reinterpret_cast<char*>(EM_ASM_PTR(
        {
            console.log('Processing shader ' + UTF8ToString($0));
            var scriptElement = document.getElementById(UTF8ToString($0));
            if (scriptElement) {
                shaderCode = scriptElement.textContent.replace('/^\s+/gm',' ').trim();
                var sourceString = Module.allocate(intArrayFromString(shaderCode), ALLOC_STACK);
                return sourceString;
            } else {
                console.log('Failed to load shader!' + 'Shader resource ' + UTF8ToString($0) + ' was not found in DOM');
                return Module.allocate(intArrayFromString(' '), ALLOC_STACK);
            }
        }, shaderName.data()
    ));
    // clang-format on

    // Return shader source
    fileContents = shaderSource;

    // As per emscripten docs, free has to be used to deallocate memory
    free(shaderSource);
}

#endif