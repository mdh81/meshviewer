#pragma once
#include "MeshViewerObject.h"
#include "Mesh.h"

namespace mv {
    // This interface facilitates dependency injection allowing tests from having to link against the mesh library
    struct IMeshFactory : MeshViewerObject {
       virtual Mesh::MeshPointer createMesh() const = 0;
    };

    struct MeshFactory : IMeshFactory {
        Mesh::MeshPointer createMesh() const override;
    };
}