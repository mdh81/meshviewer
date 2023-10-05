#include "Mesh.h"
#include "Viewer.h"
#include "ReaderFactory.h"
#include "ConfigurationReader.h"
#include "CallbackManager.h"
#include <iostream>
#include <string>
using namespace std;
using namespace mv;
using namespace mv::common;
using namespace mv::readers;
using namespace mv::config;

bool ValidateArguments(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <mesh file name 1>...<mesh file name N>" << endl;
        return false;
    }
    return true;
}

int main(int argc, char** argv) {
    if (!ValidateArguments(argc, argv)) {
        return EXIT_FAILURE;
    }

    // Set up the texture path. Cannot rely on the working directory being
    // the parent of the app's executable.
    auto parentDirectory = std::filesystem::canonical(std::filesystem::path(argv[0])).parent_path();
    ConfigurationReader::getInstance().append("texturePath", parentDirectory / "textures");

    // Create a manager to oversee lifecycle of callbacks
    CallbackManager::create();

    // Create drawable for the mesh
    Drawable::Drawables drawables;
    drawables.reserve(argc - 1);
    for (unsigned i = 1; i < argc; ++i) {
        auto spMesh =
                ReaderFactory::getReader(argv[1])->getOutput();
        drawables.push_back(std::move(spMesh));
    }
    Viewer::getInstance().add(drawables);

    // Render loop
    Viewer::getInstance().render();

    return 0;
}
