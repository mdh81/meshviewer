#include "MeshFactory.h"
#include "MeshImpl.h"

namespace mv {
    Mesh::MeshPointer MeshFactory::createMesh() const {
        return std::make_unique<MeshImpl>();
    }
}
