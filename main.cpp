#include "Mesh.h"
#include "Viewer.h"
#include "STLReader.h"
#include "ConfigurationReader.h"
#include <iostream>
#include <string>
using namespace std;
using namespace mv;
using namespace mv::common;

bool ValidateArguments(int argc, char** argv) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <mesh file name>" << endl;
        return false;
    }
    // TODO: Replace this with a Reader factory that can read a few
    // different 3D formats
    string fn = argv[1];
    string extn = fn.substr(fn.rfind('.')+1);
    if (extn != "stl" && extn != "STL") {
        cerr << "File type " << extn << " is not supported!" << endl;
        return false;
    }
    return true;
}

int main(int argc, char** argv) {
    if (!ValidateArguments(argc, argv)) {
        return EXIT_FAILURE;
    }
    unique_ptr<Mesh> mesh;
    STLReader(argv[1], config::ConfigurationReader::getInstance().getBoolean("cleanupOnImport")).getOutput(mesh);
    Viewer::getInstance().displayMesh(*mesh.get());
    return 0;
}
