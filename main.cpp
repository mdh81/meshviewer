#include "Mesh.h"
#include "ViewerFactory.h"
#include "ConfigurationReader.h"
#include "CallbackManager.h"
#include "ModelManager.h"
#include <iostream>
#include <string>
#include <filesystem>
using namespace std;
using namespace mv;
using namespace mv::common;
using namespace mv::readers;
using namespace mv::config;
using namespace mv::models;
using namespace mv::viewer;

bool ValidateArguments(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <mesh file name 1>...<mesh file name N>" << endl;
        return false;
    }
    return true;
}

void SetupTexturePath(std::string const& exeName) {
    // Set up the texture path.
#ifndef EMSCRIPTEN
    // Cannot rely on the working directory being the parent of the app's executable.
    auto parentDirectory = std::filesystem::canonical(std::filesystem::path(exeName).parent_path());
    ConfigurationReader::getInstance().append("texturePath", parentDirectory / "textures");
#else
    ConfigurationReader::getInstance().append("texturePath", "textures");
#endif
}

bool loadModels(int argc, char** argv, ModelManager& modelManager) {
#ifndef EMSCRIPTEN
    if (!ValidateArguments(argc, argv)) {
        return false;
    }
    modelManager.loadModelFiles({argv + 1, argv + argc});
#else
    modelManager.loadModelFilesFromDirectory("/testfiles");
#endif
    return true;
}

int main(int argc, char** argv) {

    // Create a manager to oversee lifecycle of callbacks
    // NOTE: Created before any other object to ensure that it gets destructed last and thereby guarantee that
    // references to it during the callback removal process are always valid
    CallbackManager::create();

    SetupTexturePath(argv[0]);

    // Load models
    ModelManager modelManager;
    if (!loadModels(argc, argv, modelManager)) {
        return EXIT_FAILURE;
    }

    // Render loop
    ViewerFactory{}.getViewer().render();

    return EXIT_SUCCESS;
}
