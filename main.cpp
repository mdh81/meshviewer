#include "Mesh.h"
#include "Viewer.h"
#include "ReaderFactory.h"
#include "ConfigurationReader.h"
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

    Renderable::Renderables renderables;
    renderables.reserve(argc - 1);
    for (unsigned i = 1; i < argc; ++i) {
        auto spMesh =
                ReaderFactory::getReader(argv[1])->
                        getOutput(ConfigurationReader::getInstance().getBoolean("cleanupOnImport"));
        renderables.push_back(std::move(spMesh));
    }
    Viewer::getInstance().add(renderables);
    Viewer::getInstance().render();
    return 0;
}
